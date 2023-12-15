#include <memory>

#include "PlasX/Vivax/White/population.hpp"
#include "pybind11/functional.h"
#include "pybind11/pybind11.h"

namespace py = pybind11;
using plasx::RealType;
using plasx::SizeType;
using plasx::vivax::white::Population;
using plasx::vivax::white::Status;

void add_pvibm_simple_module(py::module_& module) {
  pybind11::class_<Population>(module, "Population")
      .def(pybind11::init<>())
      .def("CreateIndividual",
           [](Population* self, RealType min_age, RealType max_age,
              RealType age, Status state, RealType parasite_immunity,
              RealType clinical_immunity, RealType maternal_parasite_immunity,
              RealType maternal_clinical_immunity, RealType zeta, RealType rho,
              RealType age_0, SizeType n_hypnozoites) {
             self->emplace_back(min_age, max_age, age, state, parasite_immunity,
                                clinical_immunity, maternal_parasite_immunity,
                                maternal_clinical_immunity, zeta, rho, age_0,
                                n_hypnozoites);
           })
      .def("__repr__", [](const Population& population) {
        std::string s = "[";
        for (const auto& person : population) {
          s += std::to_string(person.age_) + ", ";
        }
        s += "]";
        return s;
      });
}
