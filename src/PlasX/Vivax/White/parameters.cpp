#include "PlasX/Vivax/White/parameters.hpp"
namespace plasx {
namespace vivax {
namespace white {
Parameters::Parameters(const nlohmann::json& data)
    : eir(data.at("eir").get<double>()),
      time_step(data.at("time_step").get<double>()),
      mu_d(1.0 / data.at("life_expectancy").get<double>()),
      f(1.0 / data.at("time_to_relapse").get<double>()),
      gamma(1.0 / data.at("time_to_clear_hypnozoite").get<double>()),
      age_0(data.at("age0").get<double>()),
      rho(data.at("rho").get<double>()),
      r_P(1.0 / data.at("prophylaxis_duration").get<double>()),
      r_T(1.0 / data.at("duration_treatment").get<double>()),
      r_D(1.0 / data.at("duration_high_density_infection").get<double>()),
      r_LM(1.0 / data.at("duration_LM_infection").get<double>()),
      phiLM_min(data.at("phiLM_min").get<double>()),
      phiLM_50(data.at("phiLM_50").get<double>()),
      phiLM_max(data.at("phiLM_max").get<double>()),
      kappa_LM(data.at("kappa_LM").get<double>()),
      phiD_min(data.at("phiD_min").get<double>()),
      phiD_50(data.at("phiD_50").get<double>()),
      phiD_max(data.at("phiD_max").get<double>()),
      kappa_D(data.at("kappa_D").get<double>()),
      chiT(data.at("chiT").get<double>()),
      dPCR_min(data.at("dPCR_min").get<double>()),
      dPCR_max(data.at("dPCR_max").get<double>()),
      dPCR_50(data.at("dPCR_50").get<double>()),
      kappa_PCR(data.at("kappa_PCR").get<double>()),
      b(data.at("b").get<double>()),
      exp_rate_dt_parasite_immunity(
          std::exp(-time_step / data.at("d_parasite_immunity").get<double>())),
      exp_rate_dt_clinical_immunity(
          std::exp(-time_step / data.at("d_clinical_immunity").get<double>())),
      exp_rate_dt_maternal_immunity(
          std::exp(-time_step / data.at("d_maternal_immunity").get<double>())),
      proportion_maternal_immunity(
          data.at("proportion_maternal_immunity").get<double>()),
      end_maternal_immunity(data.at("end_maternal_immunity").get<double>()),
      refractory_period(data.at("refractory_period").get<double>()),
      c_ILM(data.at("c_ILM").get<double>()),
      c_IPCR(data.at("c_IPCR").get<double>()),
      c_ID(data.at("c_ID").get<double>()),
      c_T(data.at("c_T").get<double>()),
      biting_rate_log_mean(data.at("biting_rate_log_mean").get<double>()),
      biting_rate_log_sd(data.at("biting_rate_log_sd").get<double>()),
      max_age(data.at("max_age").get<double>()) {
  // Run all required checks for parameter constraints.
}
}  // namespace white
}  // namespace vivax
}  // namespace plasx