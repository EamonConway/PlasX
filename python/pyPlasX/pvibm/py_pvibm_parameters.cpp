#include "PlasX/Vivax/White/parameters.hpp"
#include "pybind11/pybind11.h"
namespace py = pybind11;
using namespace plasx::vivax::white;
void add_pvibm_parameter_module(pybind11::module_& module) {
  // void add_pvibm_parameter_module(py::module_& module) {
  py::class_<Parameters>(module, "HumanParameters")
      .def(py::init<int, double, double, double, double, double, double, double,
                    double, double, double, double, double, double, double,
                    double, double, double, double, double, double, double,
                    double, double, double, double, double, double, double,
                    double, double, double, double, double, double, double,
                    double, double, double, double>(),
           py::arg("num_people"), py::arg("delay"), py::arg("maternal_min_age"),
           py::arg("maternal_max_age"), py::arg("life_expectancy"),
           py::arg("time_to_relapse"), py::arg("time_to_clear_hypnozoite"),
           py::arg("age_0"), py::arg("rho"), py::arg("duration_prophylaxis"),
           py::arg("duration_treatment"),
           py::arg("duration_high_density_infection"),
           py::arg("duration_light_microscopy"), py::arg("phi_LM_min"),
           py::arg("phi_LM_50"), py::arg("phi_LM_max"), py::arg("kappa_LM"),
           py::arg("phiD_min"), py::arg("phiD_50"), py::arg("phiD_max"),
           py::arg("kappa_D"), py::arg("chiT"), py::arg("dPCR_min"),
           py::arg("dPCR_max"), py::arg("dPCR_50"), py::arg("kappa_PCR"),
           py::arg("b"), py::arg("duration_parasite_immunity"),
           py::arg("duration_clinical_immunity"),
           py::arg("duration_maternal_immunity"),
           py::arg("proportion_maternal_immunity"),
           py::arg("end_maternal_immunity"), py::arg("refractory_period"),
           py::arg("c_ILM"), py::arg("c_IPCR"), py::arg("c_ID"), py::arg("c_T"),
           py::arg("biting_rate_log_mean"), py::arg("biting_rate_log_sd"),
           py::arg("max_age"))
      .def_readwrite("num_people", &Parameters::num_people)
      .def_readwrite("delay", &Parameters::delay)
      .def_readwrite("maternal_min_age", &Parameters::min_birth_age)
      .def_readwrite("maternal_max_age", &Parameters::max_birth_age)
      .def_readwrite("death_rate", &Parameters::mu_d)
      .def_readwrite("f", &Parameters::f)
      .def_readwrite("gamma", &Parameters::gamma)
      .def_readwrite("age_0", &Parameters::age_0)
      .def_readwrite("rho", &Parameters::rho)
      .def_readwrite("prophylaxis_rate", &Parameters::r_P)
      .def_readwrite("treatment_rate", &Parameters::r_T)
      .def_readwrite("high_density_infection_rate", &Parameters::r_D)
      .def_readwrite("light_microscopy_rate", &Parameters::r_LM)
      .def_readwrite("phi_LM_min", &Parameters::phiLM_min)
      .def_readwrite("phi_LM_50", &Parameters::phiLM_50)
      .def_readwrite("phi_LM_max", &Parameters::phiLM_max)
      .def_readwrite("kappa_LM", &Parameters::kappa_LM)
      .def_readwrite("phiD_min", &Parameters::phiD_min)
      .def_readwrite("phiD_50", &Parameters::phiD_50)
      .def_readwrite("phiD_max", &Parameters::phiD_max)
      .def_readwrite("kappa_D", &Parameters::kappa_D)
      .def_readwrite("chiT", &Parameters::chiT)
      .def_readwrite("dPCR_min", &Parameters::dPCR_min)
      .def_readwrite("dPCR_max", &Parameters::dPCR_max)
      .def_readwrite("dPCR_50", &Parameters::dPCR_50)
      .def_readwrite("kappa_PCR", &Parameters::kappa_PCR)
      .def_readwrite("b", &Parameters::b)
      .def_readwrite("parasite_immunity_duration",
                     &Parameters::duration_parasite_immunity)
      .def_readwrite("clinical_immunity_duration",
                     &Parameters::duration_clinical_immunity)
      .def_readwrite("maternal_immunity_duration",
                     &Parameters::duration_maternal_immunity)
      .def_readwrite("proportion_maternal_immunity",
                     &Parameters::proportion_maternal_immunity)
      .def_readwrite("end_maternal_immunity",
                     &Parameters::end_maternal_immunity)
      .def_readwrite("refractory_period", &Parameters::refractory_period)
      .def_readwrite("c_ILM", &Parameters::c_ILM)
      .def_readwrite("c_IPCR", &Parameters::c_IPCR)
      .def_readwrite("c_ID", &Parameters::c_ID)
      .def_readwrite("c_T", &Parameters::c_T)
      .def_readwrite("biting_rate_log_mean", &Parameters::biting_rate_log_mean)
      .def_readwrite("biting_rate_log_sd", &Parameters::biting_rate_log_sd)
      .def_readwrite("max_age", &Parameters::max_age);
}
