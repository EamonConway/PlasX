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
using plasx::mosquito::SimpleMosquitoParameters;
using plasx::pvibm::simple_mosquito_ode_ibm_model;
using plasx::vivax::white::Parameters;
using plasx::vivax::white::Population;
using MosquitoState =
    std::unordered_map<plasx::MosquitoSpecies, std::array<RealType, 3>>;
using MosquitoParameters =
    std::unordered_map<plasx::MosquitoSpecies, SimpleMosquitoParameters>;

void add_pvibm_model_module(py::module_& module) {
  module.def(
      "run",
      [](double dt, double t0, double t1, double initial_eir,
         Population& population, const Parameters& individual_parameters,
         const MosquitoState& initial_state,
         const MosquitoParameters& mosquito_parameters) {
        return simple_mosquito_ode_ibm_model(
            t0, t1, dt, initial_eir, population, individual_parameters,
            initial_state, mosquito_parameters);
      },
      py::arg("time_step"), py::arg("start_time"), py::arg("finish_time"),
      py::arg("initial_eir"), py::arg("population"),
      py::arg("individual_parameters"), py::arg("mosquito_state"),
      py::arg("mosquito_parameters"),
      R"python(Run PVIBM with the simple mosquito model.)python");
};
