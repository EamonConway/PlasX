#include "PlasX/Vivax/White/status.hpp"
#include "PlasX/types.hpp"
#include "pybind11/pybind11.h"

PYBIND11_MODULE(status_, module) {
  namespace py = pybind11;
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
