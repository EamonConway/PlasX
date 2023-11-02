#include <iostream>
#include <stdexcept>
#include <string>

#include "PlasX/Mosquito/simple_mosquito_ode_fn.hpp"
#include "PlasX/Mosquito/simple_mosquito_parameters.hpp"
#include "odepp/ode_forward_euler.hpp"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
using namespace plasx;
namespace py = pybind11;

namespace {
auto simple_ode_mosquito_model(
    const double lambda, const double dt, const double t0, const double t1,
    const std::array<RealType, 3>& state,
    const mosquito::SimpleMosquitoParameters& parameters) {
  return odepp::ode_forward_euler(mosquito::simple_mosquito_ode, t0, t1, dt,
                                  state, lambda, parameters);
}
}  // namespace
void add_mosquito_module(py::module_& module) {
  py::class_<mosquito::SimpleMosquitoParameters>(module, "SimpleMosquitoParams")
      .def(py::init<RealType, RealType, RealType, RealType>(),
           py::arg("death_rate"), pybind11::arg("gamma"), py::arg("zeta"),
           pybind11::arg("phi"))
      .def_readwrite("death_rate", &mosquito::SimpleMosquitoParameters::mu)
      .def_readwrite("exposed_rate", &mosquito::SimpleMosquitoParameters::gamma)
      .def_readwrite("zeta", &mosquito::SimpleMosquitoParameters::zeta)
      .def_readwrite("phi", &mosquito::SimpleMosquitoParameters::phi)
      .def("__repr__", [](const mosquito::SimpleMosquitoParameters& x) {
        return "{\n death_rate: " + std::to_string(x.mu) +
               ",\n exposed_rate: " + std::to_string(x.gamma) +
               ",\n zeta: " + std::to_string(x.zeta) +
               ",\n phi: " + std::to_string(x.phi) + "\n}";
      });
  module.def("mosquito_model", &simple_ode_mosquito_model,
             "Run the mosquito model that is used within PVIBM.");
}
