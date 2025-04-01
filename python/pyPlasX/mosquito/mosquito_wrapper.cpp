#include <type_traits>

#include "PlasX/Mosquito/multispecies_mosquito_model.hpp"
#include "PlasX/Mosquito/simple_mosquito_ode_fn.hpp"
#include "PlasX/Mosquito/simple_mosquito_parameters.hpp"
#include "odepp/ode_forward_euler.hpp"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

namespace {
using namespace plasx;
using plasx::mosquito::mosquito_ode_model;
using plasx::mosquito::simple_mosquito_ode;
using plasx::mosquito::SimpleMosquitoParameters;

auto simple_ode_mosquito_model(const double lambda, const double dt,
                               const double t0, const double t1,
                               const std::array<RealType, 3>& state,
                               const SimpleMosquitoParameters& parameters) {
  return odepp::ode_forward_euler(simple_mosquito_ode, t0, t1, dt, state,
                                  lambda, parameters);
}

auto multispecies_simple_ode_mosquito_model(
    double lambda, double dt, double t0, double t1,
    const std::unordered_map<MosquitoSpecies, std::array<RealType, 3>>& state,
    const std::unordered_map<MosquitoSpecies, SimpleMosquitoParameters>&
        parameters) {
  auto CalculateFoi = [](const std::array<RealType, 3>& state,
                         const SimpleMosquitoParameters&) { return state[2]; };

  using ReturnType = std::invoke_result_t<
      plasx::mosquito::MultiSpeciesMosquitoOdeFn, RealType&, RealType, RealType,
      decltype(CalculateFoi), decltype(simple_mosquito_ode), decltype(state),
      decltype(parameters)>;

  auto output_state = state;
  auto tout = std::vector<RealType>();
  tout.reserve((t1 - t0) / dt);
  auto yout = std::vector<ReturnType>();
  yout.reserve((t1 - t0) / dt);

  tout.emplace_back(t0);
  yout.emplace_back(mosquito_ode_model(t0, 0.0, lambda, CalculateFoi,
                                       simple_mosquito_ode, state, parameters));
  while (t0 < t1) {
    const auto& ref = yout.emplace_back(
        mosquito_ode_model(t0, dt, lambda, CalculateFoi, simple_mosquito_ode,
                           output_state, parameters));
    tout.emplace_back(t0);
    output_state = ref.first;
  }
  return std::make_pair(tout, yout);
}
}  // namespace

PYBIND11_MODULE(_mosquito, module) {
  using namespace plasx;
  using plasx::mosquito::mosquito_ode_model;
  using plasx::mosquito::simple_mosquito_ode;
  using plasx::mosquito::SimpleMosquitoParameters;
  namespace py = pybind11;

  py::class_<SimpleMosquitoParameters>(module, "SimpleMosquitoParameters")
      .def(py::init<RealType, RealType, RealType, RealType>(),
           py::arg("death_rate"), py::arg("gamma"), py::arg("zeta"),
           py::arg("phi"))
      .def_readwrite("death_rate", &SimpleMosquitoParameters::mu)
      .def_readwrite("exposed_rate", &SimpleMosquitoParameters::gamma)
      .def_readwrite("zeta", &SimpleMosquitoParameters::zeta)
      .def_readwrite("phi", &SimpleMosquitoParameters::phi)
      .def("__repr__", [](const SimpleMosquitoParameters& x) {
        return "{\n death_rate: " + std::to_string(x.mu) +
               ",\n exposed_rate: " + std::to_string(x.gamma) +
               ",\n zeta: " + std::to_string(x.zeta) +
               ",\n phi: " + std::to_string(x.phi) + "\n}";
      });

  module.def("mosquito_model", &simple_ode_mosquito_model,
             "Run the mosquito model that is used within PVIBM.");
  module.def("ms_mosquito_model", &multispecies_simple_ode_mosquito_model,
             "Run the mosquito model that is used within PVIBM.");
}
