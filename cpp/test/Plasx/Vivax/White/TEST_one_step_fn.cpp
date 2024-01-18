#include "PlasX/Vivax/White/one_step_fn.hpp"
#include "PlasX/Vivax/White/population.hpp"
#include "PlasX/random.hpp"
#include "PlasX/udl.hpp"
#include "gtest/gtest.h"

namespace plasx {
namespace vivax {
namespace white {
// Json string literal for testing purposes.
namespace {
constexpr auto json_file = R"({
"num_people": 2000,
"eir":1.0,
"time_step":1.0,
"delay":10.0,
"min_birth_age": 6570.0,
"max_birth_age": 14600.0,
"life_expectancy": 10000000000000000.0,
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
})";
}

TEST(one_step, timestep_update) {
  auto params_json = nlohmann::json::parse(json_file);
  Parameters params(params_json);
  auto t0 = 0.0;
  auto t = t0;
  auto dt = 1.0;
  auto population = Population();
  population.emplace_back(params.min_birth_age, params.max_birth_age, 103.0_yrs,
                          Status::S, 0.0, 0.0, 1.0, 0.0, 1.0);

  [[maybe_unused]] auto return_value = one_step(t, dt, 1.0, population, params);
  // Run a second timestep for test.
  return_value = one_step(t, dt, 1.0, population, params);
};

TEST(one_step, max_age_death) {
  // Parameter file loading - we assume that the tests are run from the bin
  // folder.
  auto params_json = nlohmann::json::parse(json_file);
  Parameters params(params_json);
  auto t0 = 0.0;
  auto dt = 1.0;
  auto population = Population();
  for (auto i = 0; i < 10; ++i) {
    population.emplace_back(100.0_yrs, 101.0_yrs, 103.0_yrs, Status::S, 0.0,
                            0.0, 0.0, 0.0, 1.0);
  }

  [[maybe_unused]] auto t = one_step(t0, dt, 1.0, population, params);

  // As the only individual is above the max age, they should die and be
  // replaced by a new individual.
  for (auto person : population) {
    EXPECT_EQ(person.age_, 0.0);
  }
};

TEST(one_step, ageing) {
  // Parameter file loading - we assume that the tests are run from the bin
  // folder.
  auto initial_age = 10.0_yrs;
  auto params_json = nlohmann::json::parse(json_file);
  params_json.at("life_expectancy") = std::numeric_limits<double>::max();
  Parameters params(params_json);
  auto t0 = 0.0;
  auto dt = 150.0;
  auto population = Population();
  for (auto i = 0; i < 100; ++i) {
    population.emplace_back(100.0_yrs, 101.0_yrs, initial_age, Status::S, 0.0,
                            0.0, 0.0, 0.0, 1.0);
  }
  one_step(t0, dt, 1.0, population, params);

  // Check that they aged.
  for (auto person : population) {
    EXPECT_EQ(person.age_, initial_age + dt);
  }
};

TEST(one_step, zero_eir) {
  auto gen_age = std::exponential_distribution<RealType>(1.0 / 25.0);
  auto params_json = nlohmann::json::parse(json_file);
  params_json.at("life_expectancy") = std::numeric_limits<double>::max();
  Parameters params(params_json);
  auto t0 = 0.0;
  auto dt = 150.0;
  auto population = Population();
  for (auto i = 0; i < 100; ++i) {
    population.emplace_back(100.0_yrs, 101.0_yrs, gen_age(generator), Status::S,
                            0.0, 0.0, 0.0, 0.0, 1.0);
  }
  auto [output, total_foi] = one_step(t0, dt, 1.0, population, params);
  for (auto [state, value] : output) {
    if (state == Status::S) {
      EXPECT_EQ(std::size_t(value), population.size());
    } else {
      EXPECT_EQ(value, 0);
    }
  }
  EXPECT_EQ(total_foi, 0.0);
}

TEST(one_step, all_death) {
  // Load parameter details and ensure death.
  auto params_json = nlohmann::json::parse(json_file);
  // params_json.at("life_expectancy") =
  std::numeric_limits<RealType>::min();
  Parameters params(params_json);
  params.mu_d = std::numeric_limits<RealType>::max();
  auto t0 = 0.0;
  auto dt = 150.0;

  auto population = Population();
  for (auto i = 0; i < 100; ++i) {
    population.emplace_back(100.0_yrs, 101.0_yrs, 0.0, Status::S, 0.0, 0.0, 0.0,
                            0.0, 0.0);
  }
  auto output = one_step(t0, dt, 0.0, population, params);
  for (auto person : population) {
    EXPECT_EQ(person.age_, 0.0);
  }
};
}  // namespace white
}  // namespace vivax
}  // namespace plasx
