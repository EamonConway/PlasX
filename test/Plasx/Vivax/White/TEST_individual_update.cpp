#include "PlasX/Vivax/White/individual_update.hpp"
#include "PlasX/random.hpp"
#include "gtest/gtest.h"
namespace plasx {
namespace vivax {
namespace white {
namespace {
// Json string literal for testing purposes.
constexpr auto json_file = R"({
"num_people": 2000,
"eir":1.0,
"time_step":1.0,
"delay":10.0,
"min_birth_age": 6570.0,
"max_birth_age": 14600.0,
"life_expectancy": 0.0,
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
}  // namespace

class IndividualOneStepTests : public ::testing::Test {
 protected:
  void SetUp() override {
    // Set up common resources before each test case
    zeta = 1.0;
    Parameters temp_params(nlohmann::json::parse(json_file));
    temp_params.mu_d = 0.0;  // Disable death.
    params = std::make_unique<const Parameters>(temp_params);
    base_person = std::make_unique<const Individual<PVivax>>(
        0.0, Status::S, 0.0, 0.0, 0.0, 0.0, zeta, params->rho, 365.0 * 15.0, 0);
    t0 = 0.0;
    dt = 1.0;
  }

  void TearDown() override {
    // Nothing required to be done as using smart pointers and not allocating
    // memory.
  }

  std::unique_ptr<const Parameters> params;
  std::unique_ptr<const Individual<PVivax>> base_person;
  RealType dt;
  RealType t0;
  RealType zeta;
};

TEST_F(IndividualOneStepTests, MaxAge) {
  auto person = *base_person;
  // Modify
  person.age_ = params->max_age + 1.0;
  auto [isDead, c] = IndividualOneStep(t0, dt, person, *params, 1.0);
  EXPECT_TRUE(isDead);
  EXPECT_EQ(c, 0.0);
}

TEST_F(IndividualOneStepTests, randomness) {
  auto local_params = *params;
  (void)local_params;
  generator.seed(0);
  //for (auto i = 0; i < 100; ++i) {
  //  std::cout << genunf_std(generator) << std::endl;
  //}
  FAIL();
};

class NothingHappens : public IndividualOneStepTests,
                       public ::testing::WithParamInterface<Status> {};

TEST_P(NothingHappens, State) {
  auto local_params = *params;
  local_params.r_T = 0.0;
  generator.seed(0);
  auto person = *base_person;
  auto state = GetParam();
  person.status_.current_ = state;
  auto age = person.age_;
  auto [isDead, c] = IndividualOneStep(0.0, dt, person, local_params, 0.0);
  EXPECT_FALSE(isDead);
  EXPECT_EQ(person.age_, age + dt);
  EXPECT_EQ(person.status_.current_, state);
  EXPECT_EQ(person.status_.getClinicalImmunity(), 0.0);
  EXPECT_EQ(person.status_.getNumHypnozoites(), 0);
  EXPECT_EQ(
      person.status_.getOmega(),
      1.0 - local_params.rho * std::exp(-(age + dt) / local_params.age_0));
  EXPECT_EQ(person.status_.getParasiteImmunity(), 0.0);
  EXPECT_EQ(person.status_.getZeta(), zeta);

  // Check inaccesible private data.
  const auto data = person.status_.copyPrivateData();
  EXPECT_EQ(data.time_refractory_period_over,
            std::numeric_limits<double>::lowest());
  EXPECT_EQ(data.infection_queue.size(), 0);
}

INSTANTIATE_TEST_SUITE_P(IndividualOneStepTests, NothingHappens,
                         ::testing::Values(Status::S, Status::I_PCR,
                                           Status::I_D, Status::I_LM, Status::T,
                                           Status::P));

TEST_F(IndividualOneStepTests, SusceptibleUpdateNewInfection) {
  auto person = *base_person;
  auto age = person.age_;
  auto [isDead, c] = IndividualOneStep(t0, dt, person, *params, 10.0);

  // The only thing that happened is an infection was queued.
  EXPECT_FALSE(isDead);
  EXPECT_EQ(person.age_, age + dt);
  EXPECT_EQ(person.status_.current_, Status::S);
  EXPECT_EQ(person.status_.getClinicalImmunity(), 0.0);
  EXPECT_EQ(person.status_.getNumHypnozoites(), 0);
  EXPECT_EQ(person.status_.getOmega(),
            1.0 - params->rho * std::exp(-(age + dt) / params->age_0));
  EXPECT_EQ(person.status_.getParasiteImmunity(), 0.0);
  EXPECT_EQ(person.status_.getZeta(), zeta);

  // Check inaccesible private data.
  const auto data = person.status_.copyPrivateData();
  EXPECT_EQ(data.time_refractory_period_over,
            std::numeric_limits<double>::lowest());
  EXPECT_EQ(data.infection_queue.size(), 1);
}

TEST_F(IndividualOneStepTests, SusceptibleUpdateTriggerInfection) {
  generator.seed(0);
  auto person = *base_person;
  person.status_.queueInfection(t0, 1.0, 1.0, 10.0);
  const auto pre_data = person.status_.copyPrivateData();
  const auto time_infection = pre_data.infection_queue.top().getInfectionTime();
  auto age = person.age_;
  auto [isDead, c] =
      IndividualOneStep(time_infection, dt, person, *params, 0.0);

  // An infection was triggered.
  EXPECT_FALSE(isDead);
  EXPECT_EQ(person.age_, age + dt);
  EXPECT_EQ(person.status_.current_, Status::I_D);
  EXPECT_EQ(person.status_.getClinicalImmunity(), 1.0);
  EXPECT_EQ(person.status_.getNumHypnozoites(), 1);
  EXPECT_EQ(person.status_.getOmega(),
            1.0 - params->rho * std::exp(-(age + dt) / params->age_0));
  EXPECT_EQ(person.status_.getParasiteImmunity(), 1.0);
  EXPECT_EQ(person.status_.getZeta(), zeta);

  // Check inaccesible private data.
  const auto data = person.status_.copyPrivateData();
  EXPECT_EQ(data.time_refractory_period_over,
            time_infection + params->refractory_period);
  EXPECT_EQ(data.infection_queue.size(), 0);
}

TEST_F(IndividualOneStepTests, SusceptibleUpdateTrigger5Infection) {
  generator.seed(0);
  auto person = *base_person;
  for (auto i = 0; i < 5; ++i) {
    person.status_.queueInfection(t0, 1.0, 1.0, 10.0);
  }
  const auto pre_data = person.status_.copyPrivateData();
  const auto time_infection = pre_data.infection_queue.top().getInfectionTime();
  auto age = person.age_;
  auto [isDead, c] =
      IndividualOneStep(time_infection, dt, person, *params, 0.0);

  // An infection was triggered.
  EXPECT_FALSE(isDead);
  EXPECT_EQ(person.age_, age + dt);
  EXPECT_EQ(person.status_.current_, Status::T);
  EXPECT_EQ(person.status_.getClinicalImmunity(), 1.0);
  EXPECT_EQ(person.status_.getNumHypnozoites(), 5);
  EXPECT_EQ(person.status_.getOmega(),
            1.0 - params->rho * std::exp(-(age + dt) / params->age_0));
  EXPECT_EQ(person.status_.getParasiteImmunity(), 1.0);
  EXPECT_EQ(person.status_.getZeta(), zeta);

  // Check inaccesible private data.
  const auto data = person.status_.copyPrivateData();
  EXPECT_EQ(data.time_refractory_period_over,
            time_infection + params->refractory_period);
  EXPECT_EQ(data.infection_queue.size(), 0);
}

TEST_F(IndividualOneStepTests, SusceptibleUpdateRelapse) {
  generator.seed(0);
  auto person = *base_person;
  person.status_.queueInfection(t0, 0.0, 1.0, 10.0);
  const auto pre_data = person.status_.copyPrivateData();
  const auto time_infection = pre_data.infection_queue.top().getInfectionTime();
  auto age = person.age_;
  auto [isDead, c] =
      IndividualOneStep(time_infection, dt, person, *params, 0.0);

  // An infection was triggered.
  EXPECT_FALSE(isDead);
  EXPECT_EQ(person.age_, age + dt);
  EXPECT_EQ(person.status_.current_, Status::I_D);
  EXPECT_EQ(person.status_.getClinicalImmunity(), 1.0);
  EXPECT_EQ(person.status_.getNumHypnozoites(), 0);
  EXPECT_EQ(person.status_.getOmega(),
            1.0 - params->rho * std::exp(-(age + dt) / params->age_0));
  EXPECT_EQ(person.status_.getParasiteImmunity(), 1.0);
  EXPECT_EQ(person.status_.getZeta(), zeta);

  // Check inaccesible private data.
  const auto data = person.status_.copyPrivateData();
  EXPECT_EQ(data.time_refractory_period_over,
            time_infection + params->refractory_period);
  EXPECT_EQ(data.infection_queue.size(), 0);
}

class InfectionIPCRPath : public IndividualOneStepTests,
                          public ::testing::WithParamInterface<Status> {};

TEST_P(InfectionIPCRPath, APUpdate) {
  auto local_params = *params;
  local_params.phiLM_min = 0.0;
  local_params.phiLM_max = local_params.phiLM_min;
  auto person = *base_person;
  person.status_.current_ = GetParam();
  person.status_.queueInfection(t0, 1.0, 1.0, 10.0);
  const auto pre_data = person.status_.copyPrivateData();
  const auto time_infection = pre_data.infection_queue.top().getInfectionTime();
  auto age = person.age_;
  auto [isDead, c] =
      IndividualOneStep(time_infection, dt, person, local_params, 0.0);

  // An infection was triggered.
  EXPECT_FALSE(isDead);
  EXPECT_EQ(person.age_, age + dt);
  EXPECT_EQ(person.status_.current_, Status::I_PCR);
  EXPECT_EQ(person.status_.getClinicalImmunity(), 1.0);
  EXPECT_EQ(person.status_.getNumHypnozoites(), 1);
  EXPECT_EQ(
      person.status_.getOmega(),
      1.0 - local_params.rho * std::exp(-(age + dt) / local_params.age_0));
  EXPECT_EQ(person.status_.getParasiteImmunity(), 1.0);
  EXPECT_EQ(person.status_.getZeta(), zeta);

  // Check inaccesible private data.
  const auto data = person.status_.copyPrivateData();
  EXPECT_EQ(data.time_refractory_period_over,
            time_infection + local_params.refractory_period);
  EXPECT_EQ(data.infection_queue.size(), 0);
}

INSTANTIATE_TEST_SUITE_P(IndividualOneStepTests, InfectionIPCRPath,
                         ::testing::Values(Status::S, Status::I_PCR));

class InfectionILMPath : public IndividualOneStepTests,
                         public ::testing::WithParamInterface<Status> {};

TEST_P(InfectionILMPath, ACUpdate) {
  auto local_params = *params;
  local_params.phiLM_min = 1.0;
  local_params.phiLM_max = local_params.phiLM_min;
  local_params.phiD_min = 0.0;
  local_params.phiD_max = local_params.phiD_min;
  auto person = *base_person;
  person.status_.current_ = GetParam();
  person.status_.queueInfection(t0, 1.0, 1.0, 10.0);
  const auto pre_data = person.status_.copyPrivateData();
  const auto time_infection = pre_data.infection_queue.top().getInfectionTime();
  auto age = person.age_;
  auto [isDead, c] =
      IndividualOneStep(time_infection, dt, person, local_params, 0.0);

  // An infection was triggered.
  EXPECT_FALSE(isDead);
  EXPECT_EQ(person.age_, age + dt);
  EXPECT_EQ(person.status_.current_, Status::I_LM);
  EXPECT_EQ(person.status_.getClinicalImmunity(), 1.0);
  EXPECT_EQ(person.status_.getNumHypnozoites(), 1);
  EXPECT_EQ(
      person.status_.getOmega(),
      1.0 - local_params.rho * std::exp(-(age + dt) / local_params.age_0));
  EXPECT_EQ(person.status_.getParasiteImmunity(), 1.0);
  EXPECT_EQ(person.status_.getZeta(), zeta);

  // Check inaccesible private data.
  const auto data = person.status_.copyPrivateData();
  EXPECT_EQ(data.time_refractory_period_over,
            time_infection + local_params.refractory_period);
  EXPECT_EQ(data.infection_queue.size(), 0);
}

INSTANTIATE_TEST_SUITE_P(IndividualOneStepTests, InfectionILMPath,
                         ::testing::Values(Status::S, Status::I_PCR,
                                           Status::I_LM));

class InfectionIDPath : public IndividualOneStepTests,
                        public ::testing::WithParamInterface<Status> {};

TEST_P(InfectionIDPath, ACUpdate) {
  auto local_params = *params;
  local_params.phiLM_min = 1.0;
  local_params.phiLM_max = local_params.phiLM_min;
  local_params.phiD_min = 1.0;
  local_params.phiD_max = local_params.phiD_min;
  local_params.chiT = 0.0;
  auto person = *base_person;
  person.status_.current_ = GetParam();
  person.status_.queueInfection(t0, 1.0, 1.0, 10.0);
  const auto pre_data = person.status_.copyPrivateData();
  const auto time_infection = pre_data.infection_queue.top().getInfectionTime();
  auto age = person.age_;
  auto [isDead, c] =
      IndividualOneStep(time_infection, dt, person, local_params, 0.0);

  // An infection was triggered.
  EXPECT_FALSE(isDead);
  EXPECT_EQ(person.age_, age + dt);
  EXPECT_EQ(person.status_.current_, Status::I_D);
  EXPECT_EQ(person.status_.getClinicalImmunity(), 1.0);
  EXPECT_EQ(person.status_.getNumHypnozoites(), 1);
  EXPECT_EQ(
      person.status_.getOmega(),
      1.0 - local_params.rho * std::exp(-(age + dt) / local_params.age_0));
  EXPECT_EQ(person.status_.getParasiteImmunity(), 1.0);
  EXPECT_EQ(person.status_.getZeta(), zeta);

  // Check inaccesible private data.
  const auto data = person.status_.copyPrivateData();
  EXPECT_EQ(data.time_refractory_period_over,
            time_infection + local_params.refractory_period);
  EXPECT_EQ(data.infection_queue.size(), 0);
}

INSTANTIATE_TEST_SUITE_P(IndividualOneStepTests, InfectionIDPath,
                         ::testing::Values(Status::S, Status::I_PCR,
                                           Status::I_LM, Status::I_D));

class InfectionTPath : public IndividualOneStepTests,
                       public ::testing::WithParamInterface<Status> {};

TEST_P(InfectionTPath, ACUpdate) {
  auto local_params = *params;
  local_params.phiLM_min = 1.0;
  local_params.phiLM_max = local_params.phiLM_min;
  local_params.phiD_min = 1.0;
  local_params.phiD_max = local_params.phiD_min;
  local_params.chiT = 1.0;
  auto person = *base_person;
  auto state = GetParam();
  person.status_.current_ = state;
  person.status_.queueInfection(t0, 1.0, 1.0, 10.0);
  const auto pre_data = person.status_.copyPrivateData();
  const auto time_infection = pre_data.infection_queue.top().getInfectionTime();
  auto age = person.age_;
  auto [isDead, c] =
      IndividualOneStep(time_infection, dt, person, local_params, 0.0);

  // An infection was triggered.
  EXPECT_FALSE(isDead);
  EXPECT_EQ(person.age_, age + dt);
  if (state == Status::I_D) {
    EXPECT_EQ(person.status_.current_, Status::I_D);
  } else {
    EXPECT_EQ(person.status_.current_, Status::T);
  }

  EXPECT_EQ(person.status_.getClinicalImmunity(), 1.0);
  EXPECT_EQ(person.status_.getNumHypnozoites(), 1);
  EXPECT_EQ(
      person.status_.getOmega(),
      1.0 - local_params.rho * std::exp(-(age + dt) / local_params.age_0));
  EXPECT_EQ(person.status_.getParasiteImmunity(), 1.0);
  EXPECT_EQ(person.status_.getZeta(), zeta);

  // Check inaccesible private data.
  const auto data = person.status_.copyPrivateData();
  EXPECT_EQ(data.time_refractory_period_over,
            time_infection + local_params.refractory_period);
  EXPECT_EQ(data.infection_queue.size(), 0);
}

INSTANTIATE_TEST_SUITE_P(IndividualOneStepTests, InfectionTPath,
                         ::testing::Values(Status::S, Status::I_PCR,
                                           Status::I_LM, Status::I_D));

class DeathPath : public IndividualOneStepTests,
                  public ::testing::WithParamInterface<Status> {};
TEST_P(DeathPath, NoInfection) {
  auto local_params = *params;
  local_params.mu_d = std::numeric_limits<RealType>::max();
  auto person = *base_person;
  auto state = GetParam();
  person.status_.current_ = state;
  auto [isDead, c] = IndividualOneStep(t0, dt, person, local_params, 0.0);
  // An infection was triggered.
  EXPECT_TRUE(isDead);
}

TEST_P(DeathPath, InfectionStopsDeath) {
  auto local_params = *params;
  local_params.mu_d = std::numeric_limits<RealType>::max();
  auto person = *base_person;
  auto state = GetParam();
  person.status_.current_ = state;
  person.status_.queueInfection(t0, 1.0, 1.0, 10.0);
  const auto time_infection =
      person.status_.copyPrivateData().infection_queue.top().getInfectionTime();

  auto [isDead, c] =
      IndividualOneStep(time_infection, dt, person, local_params, 0.0);
  // An infection was triggered.
  EXPECT_FALSE(isDead);
}
INSTANTIATE_TEST_SUITE_P(IndividualOneStepTests, DeathPath,
                         ::testing::Values(Status::S, Status::I_PCR,
                                           Status::I_LM, Status::I_D, Status::T,
                                           Status::P));
}  // namespace white
}  // namespace vivax
}  // namespace plasx
