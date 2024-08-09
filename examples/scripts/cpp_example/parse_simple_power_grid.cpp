// compile with: g++ -O3 -I./../../../generated/cpp -I./json/single_include/nlohmann parse_simple_power_grid.cpp -o parse_simple_power_grid.exe
// requires C++17 or higher


#include <string>   // necessary for passing file names
#include <fstream>  // necessary for handling from files
#include <iostream> // necessary for printing to screen
#include <vector>   // necessary for handling vectors
#include <functional> // necessayr for using std::function

#include "timer.hpp"
#include "ctm_schemas.hpp"
#include "simple_power_grid.hpp"

auto parse_json_file(const std::string& fname)
{
  std::ifstream f(fname);
  return nlohmann::json::parse(f);
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

using ctm_schemas::CtmData;
using ctm_schemas::CtmSolution;

int main()
{
  Timer clock;
  
  // reading CTM instance data
  clock.tic();
  CtmData sys = parse_json_file("../../instances/unit_commitment_data/2020-01-27.json");
  CtmSolution sol = parse_json_file("../../instances/unit_commitment_solutions/2020-01-27_solution.json");
  clock.toc();
  std::cout << "Read UC data and solution file in " << clock.duration().count() << " milliseconds." << std::endl;
  std::cout << "System has " << sys.network.bus.size() << " buses." << std::endl; 
  //double value = sol.time_series_data.value().values.value().at(0).at(0);
  
  // creating simple power grid object
  clock.tic();
  SimplePowerGrid spg(sys, sol);
  clock.toc();
  std::cout << "Created SimplePowerGrid in " << clock.duration().count() << " milliseconds." << std::endl;
  std::cout << "System data:" << std::endl;
  std::cout << "\tBus from (len=" << spg.bus_fr.size() << "): "; print_vector(spg.bus_fr); std::cout << std::endl;
  std::cout << "\tBus to (len=" << spg.bus_to.size() << "): "; print_vector(spg.bus_to); std::cout << std::endl;
  std::cout << "\tSusceptance (len=" << spg.susceptance.size() << "): "; print_vector(spg.susceptance); std::cout << std::endl;
  std::cout << "Measurement data:" << std::endl;
  std::cout << "\tVoltage angles (len=" << spg.voltage_angle.size() << "): "; print_vector(spg.voltage_angle); std::cout << std::endl;
  

}
