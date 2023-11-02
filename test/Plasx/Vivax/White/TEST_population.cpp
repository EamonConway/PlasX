#include <algorithm>

#include "PlasX/Vivax/White/population.hpp"
#include "PlasX/udl.hpp"
#include "gtest/gtest.h"
using namespace plasx::vivax::white;
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
}

TEST(Population, TrivialConstructor) {
  auto pop = Population();
  EXPECT_EQ(pop.size(), std::size_t(0));
  EXPECT_EQ(pop.get_maternal_immunity().size(), std::size_t(1));
  EXPECT_EQ(pop.total_omega_zeta(), 0.0);
}

TEST(Population, EmplaceBack) {
  auto pop = Population();
  pop.emplace_back(1000.0, 10000.0, 0.0, Status::S, 0.0, 0.0, 0.0, 0.0, 1.0,
                   0.2, 10.0, 0);
  EXPECT_EQ(pop.size(), std::size_t(1));
  EXPECT_EQ(pop.total_omega_zeta(), 0.8);
  EXPECT_EQ(pop.get_maternal_immunity().size(), std::size_t(1));

  auto iter = pop.begin();
  EXPECT_EQ(++iter, pop.end());
}

TEST(Population, EmplaceBackOneStepBroken) {
  auto params_json = nlohmann::json::parse(json_file);
  Parameters params(params_json);
  auto population = Population();
  population.emplace_back(params.min_birth_age, params.max_birth_age, 103.0_yrs,
                          Status::S, 0.0, 0.0, 0.0, 0.0, 1.0);
  EXPECT_EQ(population.size(), std::size_t(1));
  EXPECT_EQ(population.get_maternal_immunity().size(), std::size_t(1));

  auto iter = population.begin();
  EXPECT_EQ(++iter, population.end());
  auto i = 0;
  std::for_each(population.begin(), population.end(),
                [&](auto person) { ++i; });
  EXPECT_EQ(i, 1);
}

TEST(Population, AssignmentOperator) {
  // There appears to be a problemw ith assigning an individual to a person.
  // Current bug is giving a bad access error in lldb. That indicates that there
  // is a problem with the memory. I think it triggers in the individual_dies
  // branch.
  auto params_json = nlohmann::json::parse(json_file);
  Parameters params(params_json);
  auto population = Population();
  population.emplace_back(params.min_birth_age, params.max_birth_age, 103.0_yrs,
                          Status::S, 0.0, 0.0, 0.0, 0.0, 1.0);
  EXPECT_EQ(population.size(), std::size_t(1));
  EXPECT_EQ(population.get_maternal_immunity().size(), std::size_t(1));

  auto iter = population.begin();
  auto parasite_immunity = 1.0, age_0 = 2820.0, zeta = 1.0, rho = 0.2,
       clinical_immunity = 1.0;
  *iter = Population::PersonType(
      0.0, Status::S, params.proportion_maternal_immunity * parasite_immunity,
      params.proportion_maternal_immunity * clinical_immunity, zeta, rho,
      age_0);
  std::for_each(population.begin(), population.end(),
                [&](Population::PersonType& person) {
                  person = Population::PersonType(
                      0.0, Status::S,
                      params.proportion_maternal_immunity * parasite_immunity,
                      params.proportion_maternal_immunity * clinical_immunity,
                      zeta, rho, age_0);
                });
}
}  // namespace white
}  // namespace vivax
}  // namespace plasx
