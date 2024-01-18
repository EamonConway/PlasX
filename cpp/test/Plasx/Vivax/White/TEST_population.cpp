#include <algorithm>

#include "PlasX/Vivax/White/parameters.hpp"
#include "PlasX/Vivax/White/population.hpp"
#include "PlasX/udl.hpp"
#include "gtest/gtest.h"
#include "nlohmann/json.hpp"
using namespace plasx;
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
}

TEST(Population, EmplaceBackRepeat) {
  auto pop = Population();
  while (pop.size() != 100) {
    pop.emplace_back(1000.0, 10000.0, 0.0, Status::S, 0.0, 0.0, 0.0, 0.0, 1.0,
                     0.0, 10.0, 0);
  }
  EXPECT_EQ(pop.size(), std::size_t(100));
  EXPECT_EQ(pop.total_omega_zeta(), 100.0);
  EXPECT_EQ(pop.get_maternal_immunity().size(), std::size_t(1));
}

TEST(Population, Iter) {
  auto params_json = nlohmann::json::parse(json_file);
  Parameters params(params_json);
  auto population = Population();
  population.emplace_back(params.min_birth_age, params.max_birth_age, 103.0_yrs,
                          Status::S, 0.0, 0.0, 0.0, 0.0, 1.0);
  EXPECT_EQ(++population.begin(), population.end());
}

TEST(Population, ConstIter) {
  auto params_json = nlohmann::json::parse(json_file);
  Parameters params(params_json);
  auto population = Population();
  population.emplace_back(params.min_birth_age, params.max_birth_age, 103.0_yrs,
                          Status::S, 0.0, 0.0, 0.0, 0.0, 1.0);
  const auto const_population = population;
  EXPECT_TRUE(std::is_const<std::remove_reference<
                  decltype(*const_population.end())>::type>::value);
  EXPECT_TRUE(std::is_const<std::remove_reference<
                  decltype(*const_population.begin())>::type>::value);
}

TEST(Population, AssignmentOperator) {
  auto params_json = nlohmann::json::parse(json_file);
  Parameters params(params_json);
  auto population = Population();
  population.emplace_back(params.min_birth_age, params.max_birth_age, 103.0_yrs,
                          Status::S, 0.0, 0.0, 0.0, 0.0, 1.0);
  EXPECT_EQ(population.size(), std::size_t(1));
  EXPECT_EQ(population.get_maternal_immunity().size(), std::size_t(1));
  auto iter = population.begin();
  const auto parasite_immunity = 1.0, age_0 = 2820.0, zeta = 1.0, rho = 0.2,
             clinical_immunity = 1.0;
  const auto individual = Population::PersonType(
      0.0, Status::S, params.proportion_maternal_immunity * parasite_immunity,
      params.proportion_maternal_immunity * clinical_immunity, zeta, rho,
      age_0);
  *iter = individual;
  EXPECT_EQ(*iter, individual);

  std::for_each(population.begin(), population.end(),
                [&](Population::PersonType& person) { person = individual; });

  std::for_each(
      population.begin(), population.end(),
      [&](Population::PersonType& person) { EXPECT_EQ(person, individual); });
}

TEST(Population, MaternalImmunityCheck) {
  auto pop = Population();
  pop.emplace_back(10.0, 10000.0, 11.0, Status::S, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                   10.0, 0);
  EXPECT_EQ(pop.get_maternal_immunity().size(), std::size_t(1));
  pop.emplace_back(12.0, 10000.0, 11.0, Status::S, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                   10.0, 0);
  EXPECT_EQ(pop.get_maternal_immunity().size(), std::size_t(1));
  pop.emplace_back(12.0, 10000.0, 21.0, Status::S, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                   10.0, 0);
  EXPECT_EQ(pop.get_maternal_immunity().size(), std::size_t(2));
  pop.emplace_back(12.0, 20.0, 21.0, Status::S, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                   10.0, 0);
  EXPECT_EQ(pop.get_maternal_immunity().size(), std::size_t(2));
  while (pop.size() != 100) {
    pop.emplace_back(1000.0, 10000.0, 1001.0, Status::S, 0.0, 0.0, 0.0, 0.0,
                     1.0, 0.0, 10.0, 0);
  }
  EXPECT_EQ(pop.size(), std::size_t(100));
  EXPECT_EQ(pop.total_omega_zeta(), 100.0);
  EXPECT_EQ(pop.get_maternal_immunity().size(), std::size_t(98));
}

TEST(Population, PersonAndPopulationCheck) {
  auto pop = Population();
  pop.emplace_back(10.0, 10000.0, 11.0, Status::S, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                   10.0, 0);
  auto& state = pop.begin()->status_;
  EXPECT_EQ(state.getOmega() * state.getZeta(), pop.total_omega_zeta());
}
