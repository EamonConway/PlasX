#include "PlasX/Vivax/White/status.hpp"
#include "PlasX/types.hpp"
#include "pybind11/pybind11.h"

namespace py = pybind11;
void add_status_module(py::module_& module) {
  using namespace plasx::vivax::white;
  using plasx::RealType;
  py::enum_<Status>(module, "Status")
      .value("S", Status::S)
      .value("I_LM", Status::I_LM)
      .value("I_PCR", Status::I_PCR)
      .value("I_D", Status::I_D)
      .value("T", Status::T)
      .value("P", Status::P);
}
