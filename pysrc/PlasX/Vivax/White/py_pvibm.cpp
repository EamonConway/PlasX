#include "PlasX/Vivax/White/pvibm.hpp"
#include "nlohmann/json.hpp"
#include "pybind11/functional.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
namespace plasx {
namespace vivax {
namespace white {
// auto simple_mosquito_ibm(const double t0, const double t1, const double dt,
//                          const double eir, const pybind11::dict& dict) {
//   auto [t, human_out, mosquito_out] =
//       mosquito_ode_ibm_model(t0, t1, dt, eir, pop, params, eir);
//
//   auto invert_yout = std::unordered_map<Status, std::vector<RealType>>();
//   for (auto& time_step : human_out) {
//     for (auto& [key, value] : time_step) {
//       invert_yout[key].push_back(value);
//     }
//   }
//   return std::make_pair(t, invert_yout);
// }
}  // namespace white
}  // namespace vivax
}  // namespace plasx

void add_pvibm_simple_module(pybind11::module_& module){};
