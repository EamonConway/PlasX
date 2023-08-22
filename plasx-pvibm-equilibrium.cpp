#include <fstream>
#include <iostream>
#include <random>

#include "PlasX/Vivax/white.hpp"
#include "PlasX/random.hpp"
#include "PlasX/simulation.hpp"
#include "PlasX/types.hpp"
#include "PlasX/udl.hpp"
#include "nlohmann/json.hpp"
using namespace plasx;
namespace pvibm = vivax::white;

int main() {
  // Parameter file loading.
  auto parameter_filename = "params_example.json";
  auto json_file = std::ifstream(parameter_filename);
  auto params_json = nlohmann::json::parse(json_file);
  pvibm::Parameters params(params_json);
  const int N = 10000;
  const auto dt = params.time_step;
  const auto eir = [](double t) -> double { return 2.0; };
  // Output file information.

  // Create individuals.
  std::vector<Individual<pvibm::PVivax>> population;
  auto gen_zeta = std::lognormal_distribution<RealType>(
      params.biting_rate_log_mean, params.biting_rate_log_sd);
  for (auto i = 0; i < N; ++i) {
    population.emplace_back(0.0, pvibm::Status::S, 0.0, 0.0, 0.0, 0.0,
                            gen_zeta(generator), params.rho, params.age_0, 0);
  }
  std::cout << population.size() << std::endl;
  // Run the simulation.
  auto start = std::chrono::steady_clock::now();
  [[maybe_unused]] auto t = plasx::simulation(
      0.0_yrs, 3000.0_yrs, dt, pvibm::one_step, population, params, eir);
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

  auto output_file = std::ofstream("age_output.csv", std::ios::out);
  if (output_file.is_open()) {
    output_file << "age, zeta, parasite_immunity, clinical_immunity, "
                   "maternal_parasite_immunity, maternal_clinical_immunity, "
                   "hypnozoites, status\n";
    for (auto person : population) {
      output_file << person.age_ << ',';
      output_file << person.status_;
      output_file << '\n';
    }
  } else {
    std::cerr << "Failed to open the output file specified." << std::endl;
  }
  return EXIT_SUCCESS;
}