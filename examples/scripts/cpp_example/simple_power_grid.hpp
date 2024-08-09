#ifndef SIMPLE_POWER_GRID
#define SIMPLE_POWER_GRID

#include <string>   // necessary for passing file names
#include <iostream> // necessary for printing to screen
#include <vector>   // necessary for handling vectors
#include <functional> // necessary for using std::function
#include <algorithm> // necessary for using std::sort
#include <exception>  // necessary to handle errors

#include "ctm_schemas.hpp"

// <<<<<<<<<<<<< AUX FUNCTIONS >>>>>>>>>>>>>>>>>>>

// function to find the permutation that would sort elements according to a comparison function
std::vector<int> sortperm(const int vsize, std::function<bool(int, int)> compare)
{
  std::vector<int> perm;
  perm.reserve(vsize);
  for(int i = 0; i < vsize; i++)
  {
    perm.push_back(i);
  }
  sort(perm.begin(), perm.end(), compare);
  return perm;
}

template <typename T>
std::vector<int> sortperm(const std::vector<T>& v)
{ 
  auto compare = [&v](const int& i, const int& j) { return(v[i] < v[j]); };
  return sortperm(static_cast<int>(v.size()), compare);
}

// function to find the indexes of the elements of a vector within another vector
// returns lookin.size() for all elements not found in lookin
template <typename T>
void indexin(const std::vector<T>& lookfor, const std::vector<T>& lookin,
             std::vector<int>& indexes)
{
  
  // initialize index vector with the size of lookin (no match found)
  const int n = lookfor.size();
  const int m = lookin.size();
  if(indexes.size() == 0)
  {
    indexes.resize(n, m);
  }else if(indexes.size() != static_cast<std::vector<int>::size_type>(n))
  {
    fprintf(stderr, "[warn] size of indexes provided (%lu) different than size of elements to look for (%lu). Will resize and continue.",
            indexes.size(), lookfor.size());
    indexes.resize(n);
    for(int& index : indexes) { index = m; }
  }else{
    for(int& index : indexes) { index = m; }
  }
   
  // compute permutations of lookfor and lookin
  std::vector<int> lforperm = sortperm(lookfor);
  std::vector<int> linperm = sortperm(lookin);
  
  // loop over both arrays at once using the sorting permutations
  int j = 0;
  for(int i = 0; i < n; i++)
    {
    T lfori = lookfor[lforperm[i]];
    while(j < m && lfori > lookin[linperm[j]]){ j++; }
    T linj = lookin[linperm[j]];
    if(lfori > linj)
    {         // we reached the last lookin value, no more matches will be found
      break;
    }else if(lfori == linj)
    {        // we found a match!
      indexes[lforperm[i]] = linperm[j];
    }        // no match for current lookfor value
  }
}

template <typename T>
std::vector<int> indexin(const std::vector<T>& lookfor, const std::vector<T>& lookin)
{
  // create index vector and initialize it with the size of lookin
  std::vector<int> indexes;
  // call in-place function and return
  indexin(lookfor, lookin, indexes);
  return indexes;
}

// <<<<<<<<<<<<< END AUX FUNCTIONS >>>>>>>>>>>>>>>>>>>

inline std::string uid_to_string(const ctm_schemas::BusFr& uid)
{
  if(std::holds_alternative<int64_t>(uid))
  {
    return std::to_string(std::get<int64_t>(uid));
  }else{
    return std::get<std::string>(uid);
  }
}

template<typename T>
std::string get_uid(const T& obj) { return uid_to_string(obj.uid); }

template<typename U, typename T>
std::vector<U> get_member_vector(const std::vector<T>& vec,
                                 std::function<U(const T&)> get_member_fun)
{
  std::vector<U> out;
  out.reserve(vec.size());
  for(const T& elem: vec)
  {
    out.push_back(get_member_fun(elem));
  }
  return out;
}

class SimplePowerGrid
{
public:
  std::vector<double> voltage_angle;
  std::vector<int> bus_fr;
  std::vector<int> bus_to;
  std::vector<double> susceptance;
  SimplePowerGrid() = delete;
  SimplePowerGrid(const ctm_schemas::CtmData& sys,      // static data
                  const ctm_schemas::CtmSolution& sol,  // 'measurements'
                  const int& t = 0);                    // index of time within measurements
};

template<typename T>
std::string get_bus_fr(const T& obj) { return uid_to_string(obj.bus_fr); }

template<typename T>
std::string get_bus_to(const T& obj) { return uid_to_string(obj.bus_to); }

template<typename T>
double get_susceptance(const T& obj) { return 1.0/obj.x; }

std::string get_va_ts_uid(const ctm_schemas::SolutionBus& bus)
{
  if(std::holds_alternative<ctm_schemas::CtmSolutionSchema>(bus.va))
  {
    auto va_ref = std::get<ctm_schemas::CtmSolutionSchema>(bus.va);
    return uid_to_string(va_ref.uid);
  }else{
    throw std::runtime_error("not a time series reference");
    return "";
  }
}

SimplePowerGrid::SimplePowerGrid(const ctm_schemas::CtmData& sys,
                                 const ctm_schemas::CtmSolution& sol,
                                 const int& t)
{
  
  // get uid vectors
  auto bus_uid = get_member_vector<std::string,
                                   ctm_schemas::NetworkBus>(sys.network.bus,
                                                           get_uid<ctm_schemas::NetworkBus>);
  std::vector<std::string> bus_fr_uid;
  { // aclines
    auto bus_fr_uid_l =
      get_member_vector<std::string,
                        ctm_schemas::NetworkAcLine>(sys.network.ac_line.value(),
                                                    get_bus_fr<ctm_schemas::NetworkAcLine>);
    bus_fr_uid.insert(std::end(bus_fr_uid), std::begin(bus_fr_uid_l), std::end(bus_fr_uid_l));
  }
  { // transformers
    auto bus_fr_uid_t =
      get_member_vector<std::string,
                        ctm_schemas::NetworkTransformer>(sys.network.transformer.value(),
                                                         get_bus_fr<ctm_schemas::NetworkTransformer>);
    bus_fr_uid.insert(std::end(bus_fr_uid), std::begin(bus_fr_uid_t), std::end(bus_fr_uid_t));
  }
  std::vector<std::string> bus_to_uid;
  { // aclines
    auto bus_to_uid_l = 
      get_member_vector<std::string,
                        ctm_schemas::NetworkAcLine>(sys.network.ac_line.value(),
                                                    get_bus_to<ctm_schemas::NetworkAcLine>);
    bus_to_uid.insert(std::end(bus_to_uid), std::begin(bus_to_uid_l), std::end(bus_to_uid_l));
  }
  { // transformers
    auto bus_to_uid_t = 
      get_member_vector<std::string,
                        ctm_schemas::NetworkTransformer>(sys.network.transformer.value(),
                                                         get_bus_to<ctm_schemas::NetworkTransformer>);
    bus_to_uid.insert(std::end(bus_to_uid), std::begin(bus_to_uid_t), std::end(bus_to_uid_t));
  }
  
  // turn uids into indexes
  indexin(bus_fr_uid, bus_uid, bus_fr);
  indexin(bus_to_uid, bus_uid, bus_to);
  
  // get susceptances
  { // aclines
    auto susceptance_l = 
      get_member_vector<double,
                        ctm_schemas::NetworkAcLine>(sys.network.ac_line.value(),
                                                    get_susceptance<ctm_schemas::NetworkAcLine>);
    susceptance.insert(std::end(susceptance), std::begin(susceptance_l), std::end(susceptance_l));
  }
  { //transformers
    auto susceptance_t = 
      get_member_vector<double,
                        ctm_schemas::NetworkTransformer>(sys.network.transformer.value(),
                                                         get_susceptance<ctm_schemas::NetworkTransformer>);
    susceptance.insert(std::end(susceptance), std::begin(susceptance_t), std::end(susceptance_t));
  }
  
  // get voltage angles from solution object
  {
    auto bus_uid_sol = get_member_vector<std::string,
                                         ctm_schemas::SolutionBus>(sol.solution.bus,
                                                                   get_uid<ctm_schemas::SolutionBus>);
    auto va_ts_uid = get_member_vector<std::string,
                                       ctm_schemas::SolutionBus>(sol.solution.bus, get_va_ts_uid);
    std::vector<std::string> ts_uid;
    ts_uid.reserve(sol.time_series_data.value().uid.size());
    for(const auto& uid: sol.time_series_data.value().uid)
    {
      ts_uid.push_back(uid_to_string(uid));
    }
    auto va_ts_idxmap = indexin(va_ts_uid, ts_uid);
    auto bus_idxmap = indexin(bus_uid, bus_uid_sol);
    voltage_angle.resize(bus_uid.size());
    for(size_t i = 0; i < bus_uid.size(); i++)
    {
      auto ts_idx = va_ts_idxmap.at(bus_idxmap.at(i));
      voltage_angle.at(i) = sol.time_series_data.value().values.value().at(ts_idx).at(t);
    }
  }
  
}

#endif
