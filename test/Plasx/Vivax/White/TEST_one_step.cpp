#include <fstream>

#include "PlasX/Vivax/White/one_step.hpp"
#include "PlasX/random.hpp"
#include "PlasX/udl.hpp"
#include "gtest/gtest.h"

using namespace plasx;
namespace pvibm = plasx::vivax::white;

// Json string literal for testing purposes.
auto json_file = R"({
"eir":1.0,
"time_step":1.0,
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
})";

TEST(one_step, timestep_update) {
  auto params_json = nlohmann::json::parse(json_file);
  pvibm::Parameters params(params_json);
  auto t0 = 0.0;
  auto dt = 1.0;
  std::vector<Individual<pvibm::PVivax>> population;
  population.emplace_back(103.0_yrs, pvibm::Status::S, 0.0, 0.0, 0.0, 0.0, 1.0);
  auto t = pvibm::one_step(t0, dt, population, params, 1.0);
  EXPECT_EQ(t, t0 + dt);

  // Run a second timestep for test.
  t0 = t;
  t = pvibm::one_step(t0, dt, population, params, 1.0);
  EXPECT_EQ(t, t0 + dt);
};

TEST(one_step, max_age_death) {
  // Parameter file loading - we assume that the tests are run from the bin
  // folder.
  auto params_json = nlohmann::json::parse(json_file);
  pvibm::Parameters params(params_json);
  auto t0 = 0.0;
  auto dt = 1.0;
  std::vector<Individual<pvibm::PVivax>> population;
  for (auto i = 0; i < 10; ++i) {
    population.emplace_back(103.0_yrs, pvibm::Status::S, 0.0, 0.0, 0.0, 0.0,
                            1.0);
  }

  [[maybe_unused]] auto t = pvibm::one_step(t0, dt, population, params, 1.0);

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
  pvibm::Parameters params(params_json);
  auto t0 = 0.0;
  auto dt = 150.0;
  std::vector<Individual<pvibm::PVivax>> population;
  for (auto i = 0; i < 100; ++i) {
    population.emplace_back(initial_age, pvibm::Status::S, 0.0, 0.0, 0.0, 0.0,
                            1.0);
  }
  pvibm::one_step(t0, dt, population, params, 1.0);

  // Check that they aged.
  for (auto person : population) {
    EXPECT_EQ(person.age_, initial_age + dt);
  }
};

TEST(one_step, randomness) {
  // Set seed so that this test will have deterministic randomness.
  generator.seed(0);

  // Load parameter details and disable death.
  auto params_json = nlohmann::json::parse(json_file);
  params_json.at("life_expectancy") = std::numeric_limits<double>::max();

  std::vector<Individual<pvibm::PVivax>> population;
  for (auto i = 0; i < 100; ++i) {
    population.emplace_back(0.0, pvibm::Status::S, 0.0, 0.0, 0.0, 0.0, 0.0);
  }
};