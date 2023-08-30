#include <pybind11/pybind11.h>

#include <iostream>

#include "PlasX/types.hpp"
#include "nlohmann/json.hpp"
void add_mosquito_module(pybind11::module_& m);
void add_equilibrium_module(pybind11::module_& m);

PYBIND11_MODULE(pyPlasX, px) {
  px.doc() =
      "pyPlasX is a python wrapper for the PlasX C++ library. Full "
      "documentation of the C++ library can be found at XX";
  auto pvibm =
      px.def_submodule("pvibm",
                       R"mydelimiter(A Plasmodium Vivax Individual Based Model.
      
      Contained within this submodule is a python implementation of the
      Plasmodium Vivax Individual based model (PVIBM) of White et al (2014).
      The states that an individual can occupy are also exposed through the
      status class.)mydelimiter");
  add_mosquito_module(pvibm);
  add_equilibrium_module(pvibm);
}