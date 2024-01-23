#include "PlasX/Vivax/White/pvibm.hpp"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

PYBIND11_MODULE(_equilibrium, module) {
  namespace py = pybind11;
  using plasx::RealType;
  using plasx::vivax::white::Parameters;
  using plasx::vivax::white::Population;
  using plasx::vivax::white::Status;
  module.def(
      "run_equilibrium",
      [](double t0, double t1, double dt, double eir, Population& population,
         const Parameters& params) {
        // Run the simulation.
        auto [t, human_out, mosquito_out] =
            plasx::pvibm::equilibrium(t0, t1, dt, eir, population, params, eir);
        auto invert_yout = std::unordered_map<Status, std::vector<RealType>>();
        for (auto& time_step : human_out) {
          for (auto& [key, value] : time_step) {
            invert_yout[key].push_back(value);
          }
        }
        return std::make_pair(t, invert_yout);
      },
      py::arg("start_time"), py::arg("end_time"), py::arg("time_step"),
      py::arg("eir"), py::arg("Population"), py::arg("PopulationParameters"),
      R"python(Run the equilibrium solution
        of PVIBM with constant entomological innoculation rate
        (eir).)python");
};
