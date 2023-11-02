#include "PlasX/Vivax/White/one_step_fn.hpp"
#include "PlasX/Vivax/White/population.hpp"
#include "PlasX/Vivax/White/pvibm.hpp"
#include "PlasX/model_simulation_fn.hpp"
#include "PlasX/random.hpp"
#include "PlasX/types.hpp"
#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
namespace plasx {
namespace vivax {
namespace white {
auto equilibrium_simulation(const double t0, const double t1, const double dt,
                            const double eir, const Parameters& params) {
  // Create individuals for the simulation
  const int N = params.num_people;
  auto gen_age = std::exponential_distribution<RealType>(params.mu_d);
  std::vector<Individual<PVivax>> population;
  auto gen_zeta = std::lognormal_distribution<RealType>(
      params.biting_rate_log_mean, params.biting_rate_log_sd);
  for (auto i = 0; i < N; ++i) {
    population.emplace_back(gen_age(generator), Status::S, 0.0, 0.0, 0.0, 0.0,
                            gen_zeta(generator), params.rho, params.age_0, 0);
  }
  Population pop(std::move(population));

  // Run the simulation.
  auto [t, human_out, mosquito_out] =
      pvibm::equilibrium(t0, t1, dt, eir, pop, params, eir);
  auto invert_yout = std::unordered_map<Status, std::vector<RealType>>();
  for (auto& time_step : human_out) {
    for (auto& [key, value] : time_step) {
      invert_yout[key].push_back(value);
    }
  }
  return std::make_pair(t, invert_yout);
}
}  // namespace white
}  // namespace vivax
}  // namespace plasx

namespace py = pybind11;
void add_equilibrium_module(py::module_& module) {
  py::enum_<plasx::vivax::white::Status>(module, "Status")
      .value("S", plasx::vivax::white::Status::S)
      .value("I_LM", plasx::vivax::white::Status::I_LM)
      .value("I_PCR", plasx::vivax::white::Status::I_PCR)
      .value("I_D", plasx::vivax::white::Status::I_D)
      .value("T", plasx::vivax::white::Status::T)
      .value("P", plasx::vivax::white::Status::P);

  module.def("equilibrium", &plasx::vivax::white::equilibrium_simulation,
             R"mydelimiter(Run the equilibrium solution of PVIBM.)mydelimiter");
};
