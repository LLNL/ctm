// compile with: g++ -O3 -I./../../../generated/cpp -I./json/single_include/nlohmann parse_and_time.cpp -o parse_and_time.exe
// requires C++17 or higher


#include <string>   // necessary for passing file names
#include <fstream>  // necessary for handling from files
#include <iostream> // necessary for printing to screen
#include <vector>   // necessary for handling vectors
#include <functional> // necessayr for using std::function

#include "timer.hpp"
#include "ctm_schemas.hpp"

auto parse_json_file(const std::string& fname)
{
  std::ifstream f(fname);
  return nlohmann::json::parse(f);
}

using ctm_schemas::CtmData;
using ctm_schemas::CtmSolution;

std::string uid_to_string(const ctm_schemas::BusFr& uid)
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

template<typename T>
void print_vector(const std::vector<T>& vec)
{
  std::cout << "[";
  for(const T& elem: vec)
  {
    std::cout << " " << elem;
  }
  std::cout << " ]";
}

int main()
{
  Timer clock;
  
  clock.tic();
  CtmData sys = parse_json_file("../../instances/unit_commitment_data/2020-01-27.json");
  clock.toc();
  std::cout << "Read UC data file in " << clock.duration().count() << " milliseconds." << std::endl;
  std::cout << "System has " << sys.network.bus.size() << " buses." << std::endl; 
  auto uid_buses = get_member_vector<std::string,
                                     ctm_schemas::NetworkBus>(sys.network.bus,
                                                              get_uid<ctm_schemas::NetworkBus>); 
  std::cout << "Bus uids: ";
  print_vector(uid_buses);
  std::cout << std::endl;
  
  clock.tic();
  CtmSolution sol = parse_json_file("../../instances/unit_commitment_solutions/2020-01-27_solution.json");
  clock.toc();
  std::cout << "Read UC solution file in " << clock.duration().count() << " milliseconds." << std::endl;
}
