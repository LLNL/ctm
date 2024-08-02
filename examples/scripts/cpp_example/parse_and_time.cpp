// compile with: g++ -O3 -I./../../../generated/cpp -I./json/single_include/nlohmann parse_and_time.cpp -o parse_and_time.exe
// requires C++17 or higher


#include <string>   // necessary for passing file names
#include <fstream>  // necessary for handling from files
#include <iostream> // necessary for printing to screen

#include "timer.hpp"
#include "ctm_schemas.hpp"

auto parse_json_file(const std::string& fname)
{
  std::ifstream f(fname);
  return nlohmann::json::parse(f);
}

using ctm_schemas::CtmData;
using ctm_schemas::CtmSolution;

int main()
{
  Timer clock;
  
  clock.tic();
  CtmData sys = parse_json_file("../../instances/unit_commitment_data/2020-01-27.json");
  clock.toc();
  std::cout << "Read UC data file in " << clock.duration().count() << " milliseconds." << std::endl;
  std::cout << "System has " << sys.network.bus.size() << " buses." << std::endl; 
  
  clock.tic();
  CtmSolution sol = parse_json_file("../../instances/unit_commitment_solutions/2020-01-27_solution.json");
  clock.toc();
  std::cout << "Read UC solution file in " << clock.duration().count() << " milliseconds." << std::endl;
}
