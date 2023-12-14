#include "PlasX/Vivax/White/parameters.hpp"
namespace plasx {
namespace vivax {
namespace white {
Parameters::Parameters(
    int num_people, double delay, double min_birth_age, double max_birth_age,
    double life_expectancy, double time_to_relapse,
    double time_to_clear_hypnozoite, double age_0, double rho,
    double prophylaxis_duration, double treatment_duration,
    double high_density_infection_duration, double LM_infection_duration,
    double phiLM_min, double phiLM_50, double phiLM_max, double kappa_LM,
    double phiD_min, double phiD_50, double phiD_max, double kappa_D,
    double chiT, double dPCR_min, double dPCR_max, double dPCR_50,
    double kappa_PCR, double b, double duration_parasite_immunity,
    double duration_clinical_immunity, double duration_maternal_immunity,
    double proportion_maternal_immunity, double end_maternal_immunity,
    double refractory_period, double c_ILM, double c_IPCR, double c_ID,
    double c_T, double biting_rate_log_mean, double biting_rate_log_sd,
    double max_age)
    : num_people(num_people),
      delay(delay),
      min_birth_age(min_birth_age),
      max_birth_age(max_birth_age),
      mu_d(1.0 / life_expectancy),
      f(1.0 / time_to_relapse),
      gamma(1.0 / time_to_clear_hypnozoite),
      age_0(age_0),
      rho(rho),
      r_P(1.0 / prophylaxis_duration),
      r_T(1.0 / treatment_duration),
      r_D(1.0 / high_density_infection_duration),
      r_LM(1.0 / LM_infection_duration),
      phiLM_min(phiLM_min),
      phiLM_50(phiLM_50),
      phiLM_max(phiLM_max),
      kappa_LM(kappa_LM),
      phiD_min(phiD_min),
      phiD_50(phiD_50),
      phiD_max(phiD_max),
      kappa_D(kappa_D),
      chiT(chiT),
      dPCR_min(dPCR_min),
      dPCR_max(dPCR_max),
      dPCR_50(dPCR_50),
      kappa_PCR(kappa_PCR),
      b(b),
      duration_parasite_immunity(duration_parasite_immunity),
      duration_clinical_immunity(duration_clinical_immunity),
      duration_maternal_immunity(duration_maternal_immunity),
      proportion_maternal_immunity(proportion_maternal_immunity),
      end_maternal_immunity(end_maternal_immunity),
      refractory_period(refractory_period),
      c_ILM(c_ILM),
      c_IPCR(c_IPCR),
      c_ID(c_ID),
      c_T(c_T),
      biting_rate_log_mean(biting_rate_log_mean),
      biting_rate_log_sd(biting_rate_log_sd),
      max_age(max_age) {}

Parameters::Parameters(const nlohmann::json& json)
    : Parameters(
          json.at("num_people").get<int>(), json.at("delay").get<double>(),
          json.at("min_birth_age").get<double>(),
          json.at("max_birth_age").get<double>(),
          json.at("life_expectancy").get<double>(),
          json.at("time_to_relapse").get<double>(),
          json.at("time_to_clear_hypnozoite").get<double>(),
          json.at("age0").get<double>(), json.at("rho").get<double>(),
          json.at("prophylaxis_duration").get<double>(),
          json.at("duration_treatment").get<double>(),
          json.at("duration_high_density_infection").get<double>(),
          json.at("duration_LM_infection").get<double>(),
          json.at("phiLM_min").get<double>(), json.at("phiLM_50").get<double>(),
          json.at("phiLM_max").get<double>(), json.at("kappa_LM").get<double>(),
          json.at("phiD_min").get<double>(), json.at("phiD_50").get<double>(),
          json.at("phiD_max").get<double>(), json.at("kappa_D").get<double>(),
          json.at("chiT").get<double>(), json.at("dPCR_min").get<double>(),
          json.at("dPCR_max").get<double>(), json.at("dPCR_50").get<double>(),
          json.at("kappa_PCR").get<double>(), json.at("b").get<double>(),
          json.at("d_parasite_immunity").get<double>(),
          json.at("d_clinical_immunity").get<double>(),
          json.at("d_maternal_immunity").get<double>(),
          json.at("proportion_maternal_immunity").get<double>(),
          json.at("end_maternal_immunity").get<double>(),
          json.at("refractory_period").get<double>(),
          json.at("c_ILM").get<double>(), json.at("c_IPCR").get<double>(),
          json.at("c_ID").get<double>(), json.at("c_T").get<double>(),
          json.at("biting_rate_log_mean").get<double>(),
          json.at("biting_rate_log_sd").get<double>(),
          json.at("max_age").get<double>()) {}
}  // namespace white
}  // namespace vivax
}  // namespace plasx
