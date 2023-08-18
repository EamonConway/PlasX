#include "PlasX/Vivax/White/parameters.hpp"
namespace plasx {
namespace vivax {
namespace white {
Parameters::Parameters(const nlohmann::json& json)
    : num_people(json.at("num_people").get<int>()),
      eir(json.at("eir").get<double>()),
      time_step(json.at("time_step").get<double>()),
      delay(json.at("delay").get<double>()),
      min_birth_age(json.at("min_birth_age").get<double>()),
      max_birth_age(json.at("max_birth_age").get<double>()),
      mu_d(1.0 / json.at("life_expectancy").get<double>()),
      f(1.0 / json.at("time_to_relapse").get<double>()),
      gamma(1.0 / json.at("time_to_clear_hypnozoite").get<double>()),
      age_0(json.at("age0").get<double>()),
      rho(json.at("rho").get<double>()),
      r_P(1.0 / json.at("prophylaxis_duration").get<double>()),
      r_T(1.0 / json.at("duration_treatment").get<double>()),
      r_D(1.0 / json.at("duration_high_density_infection").get<double>()),
      r_LM(1.0 / json.at("duration_LM_infection").get<double>()),
      phiLM_min(json.at("phiLM_min").get<double>()),
      phiLM_50(json.at("phiLM_50").get<double>()),
      phiLM_max(json.at("phiLM_max").get<double>()),
      kappa_LM(json.at("kappa_LM").get<double>()),
      phiD_min(json.at("phiD_min").get<double>()),
      phiD_50(json.at("phiD_50").get<double>()),
      phiD_max(json.at("phiD_max").get<double>()),
      kappa_D(json.at("kappa_D").get<double>()),
      chiT(json.at("chiT").get<double>()),
      dPCR_min(json.at("dPCR_min").get<double>()),
      dPCR_max(json.at("dPCR_max").get<double>()),
      dPCR_50(json.at("dPCR_50").get<double>()),
      kappa_PCR(json.at("kappa_PCR").get<double>()),
      b(json.at("b").get<double>()),
      exp_rate_dt_parasite_immunity(
          std::exp(-time_step / json.at("d_parasite_immunity").get<double>())),
      exp_rate_dt_clinical_immunity(
          std::exp(-time_step / json.at("d_clinical_immunity").get<double>())),
      exp_rate_dt_maternal_immunity(
          std::exp(-time_step / json.at("d_maternal_immunity").get<double>())),
      proportion_maternal_immunity(
          json.at("proportion_maternal_immunity").get<double>()),
      end_maternal_immunity(json.at("end_maternal_immunity").get<double>()),
      refractory_period(json.at("refractory_period").get<double>()),
      c_ILM(json.at("c_ILM").get<double>()),
      c_IPCR(json.at("c_IPCR").get<double>()),
      c_ID(json.at("c_ID").get<double>()),
      c_T(json.at("c_T").get<double>()),
      biting_rate_log_mean(json.at("biting_rate_log_mean").get<double>()),
      biting_rate_log_sd(json.at("biting_rate_log_sd").get<double>()),
      max_age(json.at("max_age").get<double>()) {
  // Run all required checks for parameter constraints.
  if (time_step <= 0.0) {
    throw std::runtime_error(
        "Error in " + std::string(__func__) + ": " + std::string(__FILE__) +
        " at line " + std::to_string(__LINE__) +
        ". Please specify a timestep that is greater that 0.0");
  }
}
}  // namespace white
}  // namespace vivax
}  // namespace plasx