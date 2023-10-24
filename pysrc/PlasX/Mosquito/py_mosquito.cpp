#include "PlasX/Mosquito/simple_mosquito_ode_fn.hpp"
#include "nlohmann/json.hpp"
#include "odepp/ode_forward_euler.hpp"
#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
using namespace plasx;
namespace {
auto simple_ode_mosquito_model(const double lambda, const double dt,
                               const double t0, const double t1,
                               const std::array<RealType, 3>& state,
                               const pybind11::dict& parameters) {
  const auto params = vivax::white::SimpleMosquitoParameters{
      1.0 / 5.0, 1.0 / 5.0, 1.0 / 5.0, 1.0 / 5.0};
  return odepp::ode_forward_euler(vivax::white::simple_mosquito_ode, t0, t1, dt,
                                  state, lambda, params);
}
}  // namespace
void add_mosquito_module(pybind11::module_& module) {
  module.def("mosquito_model", &simple_ode_mosquito_model,
             "Run the mosquito model that is used within PVIBM.");
}