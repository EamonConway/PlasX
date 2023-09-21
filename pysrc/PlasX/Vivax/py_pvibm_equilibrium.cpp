#include "PlasX/Vivax/White/population.hpp"
#include "PlasX/Vivax/White/pvibm.hpp"
#include "PlasX/Vivax/white.hpp"
#include "PlasX/random.hpp"
#include "PlasX/types.hpp"
#include "PlasX/udl.hpp"
#include "nlohmann/json.hpp"
#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
namespace plasx {
namespace vivax {
namespace white {
// Hide pybind11 implementations from other source files.
namespace {
// Helper function to convert the passed in dictionary of parameters into a json
// file.
nlohmann::json Params2Json(const pybind11::dict& dict) {
  nlohmann::json json;
  for (auto [key, value] : dict) {
    auto key_string = key.cast<std::string>();
    if (key_string.compare("num_people") == 0) {
      json[key_string] = value.cast<plasx::SizeType>();
    } else {
      json[key_string] = value.cast<plasx::RealType>();
    }
  }
  return json;
};

auto equilibrium_simulation(const double t0, const double t1, const double eir,
                            const pybind11::dict& dict) {
  const auto params = Parameters(Params2Json(dict));
  const int N = params.num_people;
  const auto dt = params.time_step;

  // Create individuals for the simulation
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
}  // namespace
}  // namespace white
}  // namespace vivax
}  // namespace plasx

void add_equilibrium_module(pybind11::module_& module) {
  pybind11::enum_<plasx::vivax::white::Status>(module, "Status")
      .value("S", plasx::vivax::white::Status::S)
      .value("I_LM", plasx::vivax::white::Status::I_LM)
      .value("I_PCR", plasx::vivax::white::Status::I_PCR)
      .value("I_D", plasx::vivax::white::Status::I_D)
      .value("T", plasx::vivax::white::Status::T)
      .value("P", plasx::vivax::white::Status::P);

  module.def("equilibrium", &plasx::vivax::white::equilibrium_simulation,
             R"mydelimiter(Run the equilibrium solution of PVIBM.)mydelimiter");

  pybind11::class_<plasx::vivax::white::Parameters>(module, "Parameters")
      .def_readwrite("age_0", &plasx::vivax::white::Parameters::age_0);
};
