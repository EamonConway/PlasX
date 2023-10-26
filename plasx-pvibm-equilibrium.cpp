#include <fstream>
#include <iostream>
#include <random>

#include "PlasX/Vivax/White/population.hpp"
#include "PlasX/Vivax/White/pvibm.hpp"
#include "PlasX/random.hpp"
#include "PlasX/types.hpp"
#include "PlasX/udl.hpp"
#include "nlohmann/json.hpp"

using namespace plasx;
int main(int argc, char* argv[]) {
  // Parameter file loading - where do you want to load the input from.
  auto parameter_filename = "params_example.json";
  auto json_file = std::ifstream(parameter_filename);
  const auto params_json = nlohmann::json::parse(json_file);
  const auto params = plasx::vivax::white::Parameters(params_json);
  const int N = params.num_people;
  auto dt = 1.0_days;
  auto t0 = 0.0_yrs, t1 = 100.0_yrs;
  auto eir = 2.73972602739726 * 10000;
  // Create individuals for the simulation
  auto gen_age = std::exponential_distribution<RealType>(params.mu_d);
  std::vector<Individual<plasx::vivax::white::PVivax>> population;
  auto gen_zeta = std::lognormal_distribution<RealType>(
      params.biting_rate_log_mean, params.biting_rate_log_sd);

  for (auto i = 0; i < N; ++i) {
    population.emplace_back(gen_age(generator), plasx::vivax::white::Status::S,
                            0.0, 0.0, 0.0, 0.0, gen_zeta(generator), params.rho,
                            params.age_0, 0);
  }

  plasx::vivax::white::Population pop(std::move(population));
  // Run the simulation.
  auto [t, yout, mosquito_out] =
      pvibm::equilibrium(t0, t1, dt, eir, pop, params, eir);
  // Open the binary file for writing
  std::ofstream outfile("output.pxbin", std::ios::binary);
  const char header_info[] = "plasx";
  outfile.write(reinterpret_cast<const char*>(&header_info),
                sizeof(header_info));
  const char command_info[] = "pvibm-equilibrium";
  const uint8_t command_size = sizeof(command_info);
  outfile.write(reinterpret_cast<const char*>(&command_size),
                sizeof(command_size));
  outfile.write(reinterpret_cast<const char*>(&command_info),
                sizeof(command_info));

  for (const auto& person : pop) {
    // Write the data to the file
    const auto& output = person.status_.compressedOutput();
    const auto age = static_cast<float>(person.age_);
    outfile.write(reinterpret_cast<const char*>(&age), sizeof(age));
    outfile.write(reinterpret_cast<const char*>(&output), sizeof(output));
  }

  // Close the file
  outfile.close();

  std::ofstream output("people.csv");
  output << "age,zeta,pimm,cimm,mpimm,cmimm,nhyp,state";
  for (const auto& person : pop) {
    // Write the data to the file
    output << '\n' << person.age_ << ", " << person.status_;
  }
  output.close();

  std::ofstream time_data("prevalence.csv");
  time_data << "time,n";
  for (auto i = 0; i < yout.size(); ++i) {
    for (const auto& [x, y] : yout[i]) {
      if (x == vivax::white::Status::I_PCR) {
        time_data << '\n' << t[i];
        time_data << "," << y;
      }
    }
  }
  return EXIT_SUCCESS;
}
