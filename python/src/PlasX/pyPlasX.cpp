#include <pybind11/pybind11.h>
namespace py = pybind11;

PYBIND11_MODULE(pyPlasX, px) {
  px.doc() =
      "pyPlasX is a python wrapper for the PlasX C++ library. Full "
      "documentation of the C++ library can be found at XX";
  auto pvibm = px.def_submodule("pvibm",
                                R"py(A Plasmodium Vivax Individual Based
                                 Model.

       Contained within this submodule is a python implementation of the
       Plasmodium Vivax Individual based model (PVIBM) of White et al (2014).
       The states that an individual can occupy are also exposed through the
       status class.)py");
  auto mosquito =
      px.def_submodule("mosquito", R"py(Mosquito module for use within PlasX.

   Functions and parameters that are used to model the mosquito
   population.)py");
}
