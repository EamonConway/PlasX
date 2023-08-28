#include <pybind11/pybind11.h>

#include <iostream>

#include "PlasX/types.hpp"
#include "nlohmann/json.hpp"
void add_mosquito_module(pybind11::module_& m);

PYBIND11_MODULE(pyPlasX, px) {
  px.doc() =
      "pyPlasX is a python wrapper for the PlasX C++ library. Full "
      "documentation of the C++ library can be found at XX";

  auto px_vivax = px.def_submodule("vivax", "This is the vivax module.");
  auto px_vivax_white =
      px_vivax.def_submodule("white", "This is the white module.");
  add_mosquito_module(px_vivax_white);
}