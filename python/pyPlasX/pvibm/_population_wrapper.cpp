#include "PlasX/Vivax/White/population.hpp"
#include "pybind11/pybind11.h"

PYBIND11_MODULE(_population, module) {
  namespace py = pybind11;
  using plasx::RealType;
  using plasx::SizeType;
  using plasx::vivax::white::Population;
  using plasx::vivax::white::Status;

  py::class_<Population>(module, "Population")
      .def(py::init<>())
      .def(
          "CreateIndividual",
          [](Population* self, RealType min_age, RealType max_age, RealType age,
             Status state, RealType parasite_immunity,
             RealType clinical_immunity, RealType maternal_parasite_immunity,
             RealType maternal_clinical_immunity, RealType zeta, RealType rho,
             RealType age_0, SizeType n_hypnozoites) {
            self->emplace_back(min_age, max_age, age, state, parasite_immunity,
                               clinical_immunity, maternal_parasite_immunity,
                               maternal_clinical_immunity, zeta, rho, age_0,
                               n_hypnozoites);
          },
          py::arg("min_age"), py::arg("max_age"), py::arg("age"),
          py::arg("status"), py::arg("parasite_immunity"),
          py::arg("clinical_immunity"), py::arg("maternal_parasite_immunity"),
          py::arg("maternal_clinical_immunity"), py::arg("bite_heterogeneity"),
          py::arg("rho"), py::arg("age_0"), py::arg("n_hypnozoites"),
          R"py(
            Create an individual and place them within your
                population.The construction is done in place.)py")
      .def("__repr__", [](const Population& population) {
        std::string s = "[";
        for (const auto& person : population) {
          s += std::to_string(person.age_) + ", ";
        }
        s += "]";
        return s;
      });
}
