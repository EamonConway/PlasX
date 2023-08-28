#include <fstream>
#include <iostream>

#include "PlasX/Vivax/White/mosquito.hpp"
#include "nlohmann/json.hpp"
#include "odepp/ode_forward_euler.hpp"
#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

using namespace plasx;
namespace pvibm = vivax::white;
constexpr pvibm::MosquitoModel mosquito;

namespace {
// Helper function to convert the passed in dictionary of parameters into a json
// file.
nlohmann::json Params2Json(const pybind11::dict& dict) {
  nlohmann::json json;
  for (auto [x, y] : dict) {
    json[x.cast<std::string>()] = y.cast<plasx::RealType>();
  }
  return json;
};
}  // namespace

auto mosquito_model(const double lambda, const double kappa, const double dt,
                    const double t0, const double t1,
                    const pybind11::dict& parameters) {
  const auto params = pvibm::MosquitoParameters(Params2Json(parameters));
  std::array<RealType, 6> y0 = {1.0};
  return odepp::ode_forward_euler(mosquito, t0, t1, dt, y0, lambda, kappa,
                                  params);
}

auto seasonal_mosquito_model(
    const double lambda,
    const std::function<plasx::RealType(plasx::RealType)>& kappa,
    const double dt, const double t0, const double t1,
    const pybind11::dict& parameters) {
  const auto params = pvibm::MosquitoParameters(Params2Json(parameters));
  std::array<RealType, 6> y0 = {1.0};
  return odepp::ode_forward_euler(mosquito, t0, t1, dt, y0, lambda, kappa,
                                  params);
}

void add_mosquito_module(pybind11::module_& module) {
  module.def("mosquito_model", &mosquito_model,
             "Run the mosquito model that is used within PVIBM.");
  module.def("seasonal_mosquito_model", &seasonal_mosquito_model,
             "Run the mosquito model that is used within PVIBM with a time "
             "varying kappa function.");
};