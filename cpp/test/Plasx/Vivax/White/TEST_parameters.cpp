#include "PlasX/Vivax/White/parameters.hpp"
#include "PlasX/parse_json.hpp"
#include "gtest/gtest.h"
using namespace plasx::vivax::white;
// Json string literal for testing purposes.
namespace {
constexpr auto json_file = R"json({
"num_people": 2000,
"eir":1.0,
"time_step":1.0,
"delay":10.0,
"min_birth_age": 6570.0,
"max_birth_age": 14600.0,
"life_expectancy": 4380.0,
"time_to_relapse": 41.0,
"time_to_clear_hypnozoite": 383.0,
"age0": 2920.0,
"rho": 0.85,
"prophylaxis_duration": 5.0,
"duration_treatment": 1.0,
"duration_high_density_infection": 5.0,
"duration_LM_infection": 16.0,
"phiLM_min": 0.011,
"phiLM_50": 18.8,
"phiLM_max": 0.93,
"kappa_LM": 3.37,
"phiD_min": 0.006,
"phiD_50": 24.5,
"phiD_max": 0.96,
"kappa_D": 5.63,
"chiT": 0.5,
"dPCR_min": 10.0,
"dPCR_50": 9.9,
"dPCR_max": 52.6,
"kappa_PCR": 3.82,
"b": 0.25,
"d_parasite_immunity": 3650.0,
"d_clinical_immunity": 10950.0,
"d_maternal_immunity": 49.9,
"proportion_maternal_immunity": 0.31,
"end_maternal_immunity": 365.0,
"refractory_period": 42.4,
"c_ILM": 0.1,
"c_IPCR": 0.035,
"c_ID": 0.8,
"c_T": 0.4,
"biting_rate_log_mean": 0.0,
"biting_rate_log_sd": 1.10905365064,
"max_age": 36500.0
})json";
}

TEST(Parameters, JsonConstructor) {
  auto params_json = plasx::parse_json(json_file).value();
  Parameters params(params_json);
  EXPECT_EQ(params.num_people, params_json.at("num_people"));
  EXPECT_EQ(params.delay, params_json.at("delay"));
  EXPECT_EQ(params.min_birth_age, params_json.at("min_birth_age"));
  EXPECT_EQ(params.max_birth_age, params_json.at("max_birth_age"));
  EXPECT_EQ(params.mu_d, 1.0 / params_json.at("life_expectancy").get<double>());
  EXPECT_EQ(params.f, 1.0 / params_json.at("time_to_relapse").get<double>());
  EXPECT_EQ(params.gamma,
            1.0 / params_json.at("time_to_clear_hypnozoite").get<double>());
  EXPECT_EQ(params.age_0, params_json.at("age0"));
  EXPECT_EQ(params.rho, params_json.at("rho"));
  EXPECT_EQ(params.r_P,
            1.0 / params_json.at("prophylaxis_duration").get<double>());
  EXPECT_EQ(params.r_T,
            1.0 / params_json.at("duration_treatment").get<double>());
  EXPECT_EQ(
      params.r_D,
      1.0 / params_json.at("duration_high_density_infection").get<double>());
  EXPECT_EQ(params.r_LM,
            1.0 / params_json.at("duration_LM_infection").get<double>());
  EXPECT_EQ(params.phiLM_min, params_json.at("phiLM_min"));
  EXPECT_EQ(params.phiLM_50, params_json.at("phiLM_50"));
  EXPECT_EQ(params.phiLM_max, params_json.at("phiLM_max"));
  EXPECT_EQ(params.kappa_LM, params_json.at("kappa_LM"));
  EXPECT_EQ(params.phiD_min, params_json.at("phiD_min"));
  EXPECT_EQ(params.phiD_50, params_json.at("phiD_50"));
  EXPECT_EQ(params.phiD_max, params_json.at("phiD_max"));
  EXPECT_EQ(params.kappa_D, params_json.at("kappa_D"));
  EXPECT_EQ(params.chiT, params_json.at("chiT"));
  EXPECT_EQ(params.dPCR_min, params_json.at("dPCR_min"));
  EXPECT_EQ(params.dPCR_50, params_json.at("dPCR_50"));
  EXPECT_EQ(params.dPCR_max, params_json.at("dPCR_max"));
  EXPECT_EQ(params.kappa_PCR, params_json.at("kappa_PCR"));
  EXPECT_EQ(params.b, params_json.at("b"));
  EXPECT_EQ(params.duration_parasite_immunity,
            params_json.at("d_parasite_immunity"));
  EXPECT_EQ(params.duration_clinical_immunity,
            params_json.at("d_clinical_immunity"));
  EXPECT_EQ(params.duration_maternal_immunity,
            params_json.at("d_maternal_immunity"));
  EXPECT_EQ(params.proportion_maternal_immunity,
            params_json.at("proportion_maternal_immunity"));
  EXPECT_EQ(params.end_maternal_immunity,
            params_json.at("end_maternal_immunity"));
  EXPECT_EQ(params.refractory_period, params_json.at("refractory_period"));
  EXPECT_EQ(params.c_ILM, params_json.at("c_ILM"));
  EXPECT_EQ(params.c_IPCR, params_json.at("c_IPCR"));
  EXPECT_EQ(params.c_ID, params_json.at("c_ID"));
  EXPECT_EQ(params.c_T, params_json.at("c_T"));
  EXPECT_EQ(params.biting_rate_log_mean,
            params_json.at("biting_rate_log_mean"));
  EXPECT_EQ(params.biting_rate_log_sd, params_json.at("biting_rate_log_sd"));
  EXPECT_EQ(params.max_age, params_json.at("max_age"));
}

TEST(Parameters, ValueConstructor) {
  auto params_json = plasx::parse_json(json_file).value();
  auto params = Parameters(
      params_json.at("num_people").get<int>(),
      params_json.at("delay").get<double>(),
      params_json.at("min_birth_age").get<double>(),
      params_json.at("max_birth_age").get<double>(),
      params_json.at("life_expectancy").get<double>(),
      params_json.at("time_to_relapse").get<double>(),
      params_json.at("time_to_clear_hypnozoite").get<double>(),
      params_json.at("age0").get<double>(), params_json.at("rho").get<double>(),
      params_json.at("prophylaxis_duration").get<double>(),
      params_json.at("duration_treatment").get<double>(),
      params_json.at("duration_high_density_infection").get<double>(),
      params_json.at("duration_LM_infection").get<double>(),
      params_json.at("phiLM_min").get<double>(),
      params_json.at("phiLM_50").get<double>(),
      params_json.at("phiLM_max").get<double>(),
      params_json.at("kappa_LM").get<double>(),
      params_json.at("phiD_min").get<double>(),
      params_json.at("phiD_50").get<double>(),
      params_json.at("phiD_max").get<double>(),
      params_json.at("kappa_D").get<double>(),
      params_json.at("chiT").get<double>(),
      params_json.at("dPCR_min").get<double>(),
      params_json.at("dPCR_max").get<double>(),
      params_json.at("dPCR_50").get<double>(),
      params_json.at("kappa_PCR").get<double>(),
      params_json.at("b").get<double>(),
      params_json.at("d_parasite_immunity").get<double>(),
      params_json.at("d_clinical_immunity").get<double>(),
      params_json.at("d_maternal_immunity").get<double>(),
      params_json.at("proportion_maternal_immunity").get<double>(),
      params_json.at("end_maternal_immunity").get<double>(),
      params_json.at("refractory_period").get<double>(),
      params_json.at("c_ILM").get<double>(),
      params_json.at("c_IPCR").get<double>(),
      params_json.at("c_ID").get<double>(), params_json.at("c_T").get<double>(),
      params_json.at("biting_rate_log_mean").get<double>(),
      params_json.at("biting_rate_log_sd").get<double>(),
      params_json.at("max_age").get<double>());

  EXPECT_EQ(params.num_people, params_json.at("num_people"));
  EXPECT_EQ(params.delay, params_json.at("delay"));
  EXPECT_EQ(params.min_birth_age, params_json.at("min_birth_age"));
  EXPECT_EQ(params.max_birth_age, params_json.at("max_birth_age"));
  EXPECT_EQ(params.mu_d, 1.0 / params_json.at("life_expectancy").get<double>());
  EXPECT_EQ(params.f, 1.0 / params_json.at("time_to_relapse").get<double>());
  EXPECT_EQ(params.gamma,
            1.0 / params_json.at("time_to_clear_hypnozoite").get<double>());
  EXPECT_EQ(params.age_0, params_json.at("age0"));
  EXPECT_EQ(params.rho, params_json.at("rho"));
  EXPECT_EQ(params.r_P,
            1.0 / params_json.at("prophylaxis_duration").get<double>());
  EXPECT_EQ(params.r_T,
            1.0 / params_json.at("duration_treatment").get<double>());
  EXPECT_EQ(
      params.r_D,
      1.0 / params_json.at("duration_high_density_infection").get<double>());
  EXPECT_EQ(params.r_LM,
            1.0 / params_json.at("duration_LM_infection").get<double>());
  EXPECT_EQ(params.phiLM_min, params_json.at("phiLM_min"));
  EXPECT_EQ(params.phiLM_50, params_json.at("phiLM_50"));
  EXPECT_EQ(params.phiLM_max, params_json.at("phiLM_max"));
  EXPECT_EQ(params.kappa_LM, params_json.at("kappa_LM"));
  EXPECT_EQ(params.phiD_min, params_json.at("phiD_min"));
  EXPECT_EQ(params.phiD_50, params_json.at("phiD_50"));
  EXPECT_EQ(params.phiD_max, params_json.at("phiD_max"));
  EXPECT_EQ(params.kappa_D, params_json.at("kappa_D"));
  EXPECT_EQ(params.chiT, params_json.at("chiT"));
  EXPECT_EQ(params.dPCR_min, params_json.at("dPCR_min"));
  EXPECT_EQ(params.dPCR_50, params_json.at("dPCR_50"));
  EXPECT_EQ(params.dPCR_max, params_json.at("dPCR_max"));
  EXPECT_EQ(params.kappa_PCR, params_json.at("kappa_PCR"));
  EXPECT_EQ(params.b, params_json.at("b"));
  EXPECT_EQ(params.duration_parasite_immunity,
            params_json.at("d_parasite_immunity"));
  EXPECT_EQ(params.duration_clinical_immunity,
            params_json.at("d_clinical_immunity"));
  EXPECT_EQ(params.duration_maternal_immunity,
            params_json.at("d_maternal_immunity"));
  EXPECT_EQ(params.proportion_maternal_immunity,
            params_json.at("proportion_maternal_immunity"));
  EXPECT_EQ(params.end_maternal_immunity,
            params_json.at("end_maternal_immunity"));
  EXPECT_EQ(params.refractory_period, params_json.at("refractory_period"));
  EXPECT_EQ(params.c_ILM, params_json.at("c_ILM"));
  EXPECT_EQ(params.c_IPCR, params_json.at("c_IPCR"));
  EXPECT_EQ(params.c_ID, params_json.at("c_ID"));
  EXPECT_EQ(params.c_T, params_json.at("c_T"));
  EXPECT_EQ(params.biting_rate_log_mean,
            params_json.at("biting_rate_log_mean"));
  EXPECT_EQ(params.biting_rate_log_sd, params_json.at("biting_rate_log_sd"));
  EXPECT_EQ(params.max_age, params_json.at("max_age"));
}
