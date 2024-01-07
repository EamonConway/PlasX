#include "PlasX/Mosquito/simple_mosquito_ode_fn.hpp"
#include "PlasX/Mosquito/simple_mosquito_parameters.hpp"
#include "PlasX/Vivax/White/parameters.hpp"
#include "PlasX/Vivax/White/population.hpp"
#include "PlasX/Vivax/White/pvibm.hpp"
#include "pybind11/pybind11.h"
// The stl header is required so that the output of mosquito_ode_ibm_model will
// be given appropriate types in the python api.
#include "pybind11/stl.h"
namespace py = pybind11;
using plasx::RealType;
using plasx::SizeType;
using plasx::mosquito::simple_mosquito_ode;
using plasx::mosquito::SimpleMosquitoParameters;
using plasx::pvibm::mosquito_ode_ibm_model;
using plasx::vivax::white::Parameters;
using plasx::vivax::white::Population;
using MosquitoState =
    std::unordered_map<plasx::MosquitoSpecies, std::array<RealType, 3>>;
using MosquitoParameters =
    std::unordered_map<plasx::MosquitoSpecies, SimpleMosquitoParameters>;

namespace {
auto multispecies_simple_ode_mosquito_model(
    double dt, double t0, double t1, double initial_eir, Population& population,
    const Parameters& individual_parameters, const MosquitoState& initial_state,
    const MosquitoParameters& mosquito_parameters) {
  auto CalculateFoi = [](const std::array<RealType, 3>& state,
                         const SimpleMosquitoParameters&) { return state[2]; };
  return mosquito_ode_ibm_model(
      t0, t1, dt, initial_eir, population, individual_parameters, CalculateFoi,
      simple_mosquito_ode, initial_state, mosquito_parameters);
}
}  // namespace

void add_pvibm_model_module(py::module_& module) {
  module.def(
      "run", multispecies_simple_ode_mosquito_model, py::arg("time_step"),
      py::arg("start_time"), py::arg("finish_time"), py::arg("initial_eir"),
      py::arg("population"), py::arg("individual_parameters"),
      py::arg("mosquito_state"), py::arg("mosquito_parameters"),
      R"python(Run the mosquito model that is used within PVIBM.)python");
};
