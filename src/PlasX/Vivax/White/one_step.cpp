/**
 * @file one_step.cpp
 * @author Eamon Conway (conway.e@wehi.edu.au)
 * @brief
 * @version 0.1
 * @date 2023-06-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "PlasX/Vivax/White/one_step.hpp"

#include <cmath>
#include <utility>

#include "PlasX/population_update.hpp"
#include "PlasX/random.hpp"
#include "PlasX/udl.hpp"
// Anonymous namespace for cacheable data and functions - they
// will not be accessible from other translation  units as they should not be
// used anywhere but here.

//  - note the discrepancy with
// eir and zeta - by taking zeta from a lognormal distribution, the mean
// lambda is actually higher than what was expected. It is stated that
// the mean of the lognormal distribution is 0, which means that
// mean(zeta) should be one. This is not the case, the mean zeta is
// exp(mu + sigma^2/2), which seems to be around 1.8.
// mean(log(zeta))=0,but that does not dictate that mean(zeta) = 1.0.

namespace {
using RealType = plasx::RealType;
using Person = plasx::Individual<plasx::vivax::white::PVivax>;

class CachedVariables {
 public:
  CachedVariables(RealType omega, RealType time)
      : omega_(omega), valid_time_(time){};
  RealType omega_;
  bool isCacheInvalid(RealType t) { return t != valid_time_; }

 private:
  RealType valid_time_;
};

std::optional<CachedVariables> kcached_data;

void calculateRequiredCache(const std::vector<Person>& population, RealType rho,
                            RealType age_0, RealType t) {
  // We need to get the force of infection from mosquito to humans, then we
  // need to calculate the age normalisation constant.
  auto cacheable_omega = 0.0;
  for (const auto& person : population) {
    cacheable_omega += person.status_.getOmega();
  }
  kcached_data.emplace(cacheable_omega, t);
};

struct MaternalImmunity {
  double parasite_immunity;
  double clinical_immunity;
  MaternalImmunity(RealType parasite, RealType clinical)
      : parasite_immunity(parasite), clinical_immunity(clinical){};
};
}  // namespace

namespace plasx {
namespace vivax {
namespace white {
// Unnamed namespace for all functions with internal linkage.
namespace {
// Constant values for return types. Make explicit what should be returned from
// functions when an individual lives or dies. The prefix k is used to signify
// that it is global within this translation unit.
const bool kEXIT_INDIVIDUAL_LIVES = false;
const bool kEXIT_INDIVIDUAL_DIES = true;

static RealType HillFunction(RealType x, RealType min, RealType diff,
                             RealType x50, RealType kappa) {
  const auto denom = 1.0 + pow(x / x50, kappa);
  return min + diff / denom;
}

// Infection model has two points at which decisions are made, ACUpdate and
// APupdate.
static void ACUpdate(PVivax& state, const double& phiD, const double& chiT) {
  const auto lm_detectable_infection = genunf_std(generator) > phiD;
  if (lm_detectable_infection) {
    state.current_ = Status::I_LM;
    return;
  }

  const auto high_density_infection = genunf_std(generator) > chiT;
  if (high_density_infection) {
    state.current_ = Status::I_D;
    return;
  }

  state.current_ = Status::T;
  return;
}

static void APUpdate(PVivax& state, const double& phiLM, const double& phiD,
                     const double& chiT) {
  const auto PCR_detectable_infection = genunf_std(generator) > phiLM;
  if (PCR_detectable_infection) {
    state.current_ = Status::I_PCR;
    return;
  }
  ACUpdate(state, phiD, chiT);
  return;
};

static bool SusceptibleUpdate(PVivax& state, const Parameters& params,
                              const RealType lambda, const RealType t,
                              const RealType dt) {
  // Parameters required.
  const auto k = state.getNumHypnozoites();
  const auto f = params.f, kf = k * f, gamma = params.gamma, kgamma = k * gamma,
             mu_d = params.mu_d;

  // Check to see if a bite or relapse occurs this time step - causing an
  // infection after some delay.
  const auto prob_infection = lambda + kf;
  const auto queue_new_infection = genunf_std(generator) < prob_infection * dt;
  if (queue_new_infection) {
    state.queueInfection(t, lambda, prob_infection);
  }

  // Hill function parameters.
  const auto phiLM_min = params.phiLM_min,
             phiLM_diff = params.phiLM_max - phiLM_min,
             phiLM_50 = params.phiLM_50, kappa_LM = params.kappa_LM;
  const auto phiD_min = params.phiD_min, phiD_diff = params.phiD_max - phiD_min,
             phiD_50 = params.phiD_50, kappa_D = params.kappa_D;
  const auto current_parasite_immunity = state.getParasiteImmunity();
  const auto current_clinical_immunity = state.getClinicalImmunity();

  // Check if a prior bite becomes an active infection this timestep.
  const auto infection_starts_now =
      state.updateInfection(t, params.refractory_period);
  if (infection_starts_now) {
    // There was an infection activated, determine what happened.
    const auto phiLM = HillFunction(current_parasite_immunity, phiLM_min,
                                    phiLM_diff, phiLM_50, kappa_LM);
    const auto phiD = HillFunction(current_clinical_immunity, phiD_min,
                                   phiD_diff, phiD_50, kappa_D);
    APUpdate(state, phiLM, phiD, params.chiT);
    return kEXIT_INDIVIDUAL_LIVES;
  }

  // Something or nothing will occur.
  const auto prob_event = mu_d + kgamma;
  const auto nothing_happens = genunf_std(generator) > prob_event * dt;
  if (nothing_happens) {
    return kEXIT_INDIVIDUAL_LIVES;
  }

  // Relapse, clearance or death will occur.
  const auto r = genunf_std(generator);
  const auto individual_clears_hypnozoite = r < kgamma / prob_event;
  if (individual_clears_hypnozoite) {
    --state.getNumHypnozoites();
    return kEXIT_INDIVIDUAL_LIVES;
  }

  return kEXIT_INDIVIDUAL_DIES;
}

static bool PCRInfectionUpdate(PVivax& state, const Parameters& params,
                               const RealType lambda, const RealType t,
                               const RealType dt) {
  // Parameters required.
  const auto k = state.getNumHypnozoites();
  const auto f = params.f, kf = k * f, gamma = params.gamma, kgamma = k * gamma,
             mu_d = params.mu_d;

  // Check to see if a bite or relapse occurs this time step - causing an
  // infection after some delay.
  const auto prob_infection = lambda + kf;
  const auto queue_new_infection = genunf_std(generator) < prob_infection * dt;
  if (queue_new_infection) {
    state.queueInfection(t, lambda, prob_infection);
  }

  // Hill function parameters - calculate the immunity dependent duration of
  // infection.
  const auto phiLM_min = params.phiLM_min,
             phiLM_diff = params.phiLM_max - phiLM_min,
             phiLM_50 = params.phiLM_50, kappa_LM = params.kappa_LM;
  const auto phiD_min = params.phiD_min, phiD_diff = params.phiD_max - phiD_min,
             phiD_50 = params.phiD_50, kappa_D = params.kappa_D;
  const auto d_min = params.dPCR_min, d_diff = params.dPCR_max - d_min,
             d50 = params.dPCR_50, kappa_PCR = params.kappa_PCR;
  const auto current_clinical_immunity = state.getClinicalImmunity();
  const auto current_parasite_immunity = state.getParasiteImmunity();
  const auto dPCR =
      HillFunction(current_parasite_immunity, d_min, d_diff, d50, kappa_PCR);
  const auto r_PCR = 1.0 / dPCR;

  // Check if a prior bite becomes an active infection this timestep.
  const auto infection_active =
      state.updateInfection(t, params.refractory_period);
  if (infection_active) {
    // There was an infection activated, determine what happened.
    const auto phiLM = HillFunction(current_parasite_immunity, phiLM_min,
                                    phiLM_diff, phiLM_50, kappa_LM);
    const auto phiD = HillFunction(current_clinical_immunity, phiD_min,
                                   phiD_diff, phiD_50, kappa_D);
    // There was an infection activated, determine what happened.
    APUpdate(state, phiLM, phiD, params.chiT);
    return kEXIT_INDIVIDUAL_LIVES;
  }

  const auto prob_event = r_PCR + kgamma + mu_d;
  const auto nothing_happens = genunf_std(generator) > prob_event * dt;
  if (nothing_happens) {
    return kEXIT_INDIVIDUAL_LIVES;
  }

  // Relapse, recovery, clearance or death will occur.
  const auto r = genunf_std(generator);
  const auto individual_recovers = r < r_PCR / prob_event;
  if (individual_recovers) {
    state.current_ = Status::S;
    return kEXIT_INDIVIDUAL_LIVES;
  }

  const auto individual_clears_hypnozoite = r < (r_PCR + kgamma) / prob_event;
  if (individual_clears_hypnozoite) {
    --state.getNumHypnozoites();
    return kEXIT_INDIVIDUAL_LIVES;
  }

  return kEXIT_INDIVIDUAL_DIES;
}

static bool LightMicroscopyInfectionUpdate(PVivax& state,
                                           const Parameters& params,
                                           const RealType lambda,
                                           const RealType t,
                                           const RealType dt) {
  // Parameters required.
  const auto k = state.getNumHypnozoites();
  const auto f = params.f, kf = k * f, gamma = params.gamma, kgamma = k * gamma,
             mu_d = params.mu_d, r_LM = params.r_LM;

  // Check to see if a bite or relapse occurs this time step - causing an
  // infection after some delay.
  const auto prob_infection = lambda + kf;
  const auto queue_new_infection = genunf_std(generator) < prob_infection * dt;
  if (queue_new_infection) {
    state.queueInfection(t, lambda, prob_infection);
  }

  // Hill function parameters.
  const auto phiD_min = params.phiD_min, phiD_diff = params.phiD_max - phiD_min,
             phiD_50 = params.phiD_50, kappa_D = params.kappa_D;
  const auto current_clinical_immunity = state.getClinicalImmunity();

  // Check if a prior bite becomes an active infection this timestep.
  const auto infection_active =
      state.updateInfection(t, params.refractory_period);
  if (infection_active) {
    // There was an infection activated, determine what happened.
    const auto phiD = HillFunction(current_clinical_immunity, phiD_min,
                                   phiD_diff, phiD_50, kappa_D);
    ACUpdate(state, phiD, params.chiT);
    return kEXIT_INDIVIDUAL_LIVES;
  }

  const auto prob_event = r_LM + kgamma + mu_d;
  const auto nothing_happens = genunf_std(generator) > prob_event * dt;
  if (nothing_happens) {
    return kEXIT_INDIVIDUAL_LIVES;
  }

  // Relapse, recovery, clearance or death will occur.
  const auto r = genunf_std(generator);
  const auto individual_recovers = r < r_LM / prob_event;
  if (individual_recovers) {
    state.current_ = Status::I_PCR;
    return kEXIT_INDIVIDUAL_LIVES;
  }

  const auto individual_clears_hypnozoite = r < (r_LM + kgamma) / prob_event;
  if (individual_clears_hypnozoite) {
    --state.getNumHypnozoites();
    return kEXIT_INDIVIDUAL_LIVES;
  }

  return kEXIT_INDIVIDUAL_DIES;
}

static bool HighDensityInfectionUpdate(PVivax& state, const Parameters& params,
                                       const RealType lambda, const RealType t,
                                       const RealType dt) {
  // Parameters required.
  const auto k = state.getNumHypnozoites();
  const auto f = params.f, kf = k * f, gamma = params.gamma, kgamma = k * gamma,
             mu_d = params.mu_d, r_D = params.r_D;

  // Check to see if a bite or relapse occurs this time step - causing an
  // infection after some delay.
  const auto prob_infection = lambda + kf;
  const auto queue_new_infection = genunf_std(generator) < prob_infection * dt;
  if (queue_new_infection) {
    state.queueInfection(t, lambda, prob_infection);
  }

  // Check if a prior bite becomes an active infection this timestep.
  const auto infection_active =
      state.updateInfection(t, params.refractory_period);
  if (infection_active) {
    return kEXIT_INDIVIDUAL_LIVES;
  }

  const auto prob_event = r_D + kgamma + mu_d;
  const auto nothing_happens = genunf_std(generator) > prob_event * dt;
  if (nothing_happens) {
    return kEXIT_INDIVIDUAL_LIVES;
  }

  // Recovery, clearance or death will occur.
  const auto r = genunf_std(generator);
  const auto individual_recovers = r < r_D / prob_event;
  if (individual_recovers) {
    state.current_ = Status::I_LM;
    return kEXIT_INDIVIDUAL_LIVES;
  }

  const auto individual_clears_hypnozoite = r < (r_D + kgamma) / prob_event;
  if (individual_clears_hypnozoite) {
    --state.getNumHypnozoites();
    return kEXIT_INDIVIDUAL_LIVES;
  }

  return kEXIT_INDIVIDUAL_DIES;
}

static bool TreatmentUpdate(PVivax& state, const Parameters& params,
                            const RealType lambda, const RealType t,
                            const RealType dt) {
  // Parameters required.
  const auto k = state.getNumHypnozoites();
  const auto f = params.f, kf = k * f, gamma = params.gamma, kgamma = k * gamma,
             mu_d = params.mu_d, r_T = params.r_T;

  // Check to see if a bite or relapse occurs this time step - causing an
  // infection after some delay.
  const auto prob_infection = lambda + kf;
  const auto queue_new_infection = genunf_std(generator) < prob_infection * dt;
  if (queue_new_infection) {
    state.queueInfection(t, lambda, prob_infection);
  }

  // Check if a prior bite becomes an active infection this timestep.
  const auto infection_active =
      state.updateInfection(t, params.refractory_period);
  if (infection_active) {
    return kEXIT_INDIVIDUAL_LIVES;
  }

  const auto prob_event = r_T + kgamma + mu_d;
  const auto nothing_happens = genunf_std(generator) > prob_event * dt;
  if (nothing_happens) {
    return kEXIT_INDIVIDUAL_LIVES;
  }

  // Recovery, clearance or death will occur.
  const auto r = genunf_std(generator);
  const auto individual_recovers = r < r_T / prob_event;
  if (individual_recovers) {
    state.current_ = Status::P;
    return kEXIT_INDIVIDUAL_LIVES;
  }

  const auto individual_clears_hypnozoite = r < (r_T + kgamma) / prob_event;
  if (individual_clears_hypnozoite) {
    --state.getNumHypnozoites();
    return kEXIT_INDIVIDUAL_LIVES;
  }

  return kEXIT_INDIVIDUAL_DIES;
}

static bool ProphylaxisUpdate(PVivax& state, const Parameters& params,
                              const RealType lambda, const RealType t,
                              const RealType dt) {
  // Parameters required.
  const auto k = state.getNumHypnozoites();
  const auto f = params.f, kf = k * f, gamma = params.gamma, kgamma = k * gamma,
             mu_d = params.mu_d, r_P = params.r_P;

  // Check to see if a bite or relapse occurs this time step - causing an
  // infection after some delay.
  const auto prob_infection = lambda + kf;
  const auto queue_new_infection = genunf_std(generator) < prob_infection * dt;
  if (queue_new_infection) {
    state.queueInfection(t, lambda, prob_infection);
  }

  // Check if a prior bite becomes an active infection this timestep.
  const auto infection_active =
      state.updateInfection(t, params.refractory_period);
  if (infection_active) {
    return kEXIT_INDIVIDUAL_LIVES;
  }

  const auto prob_event = r_P + kgamma + mu_d;
  const auto nothing_happens = genunf_std(generator) > prob_event * dt;
  if (nothing_happens) {
    return kEXIT_INDIVIDUAL_LIVES;
  }

  // Recovery, clearance or death will occur.
  const auto r = genunf_std(generator);
  const auto individual_recovers = r < r_P / prob_event;
  if (individual_recovers) {
    state.current_ = Status::S;
    return kEXIT_INDIVIDUAL_LIVES;
  }

  const auto individual_clears_hypnozoite = r < (r_P + kgamma) / prob_event;
  if (individual_clears_hypnozoite) {
    --state.getNumHypnozoites();
    return kEXIT_INDIVIDUAL_LIVES;
  }

  return kEXIT_INDIVIDUAL_DIES;
}

struct UpdateStateReturnType {
  bool isDead;
  double c;
};

static UpdateStateReturnType UpdateState(PVivax& state,
                                         const Parameters& params,
                                         const RealType lambda,
                                         const RealType t, const RealType dt) {
  auto output = UpdateStateReturnType{false, 0.0};
  switch (state.current_) {
    case Status::S:
      output.isDead = SusceptibleUpdate(state, params, lambda, t, dt);
      break;
    case Status::I_LM:
      output.isDead =
          LightMicroscopyInfectionUpdate(state, params, lambda, t, dt);
      output.c = params.c_ILM;
      break;
    case Status::I_PCR:
      output.isDead = PCRInfectionUpdate(state, params, lambda, t, dt);
      output.c = params.c_IPCR;
      break;
    case Status::I_D:
      output.isDead = HighDensityInfectionUpdate(state, params, lambda, t, dt);
      output.c = params.c_ID;
      break;
    case Status::T:
      output.isDead = TreatmentUpdate(state, params, lambda, t, dt);
      output.c = params.c_T;
      break;
    case Status::P:
      output.isDead = ProphylaxisUpdate(state, params, lambda, t, dt);
      break;
    default:
      throw std::logic_error("Individual is in an unknown compartment.");
  }
  return output;
};
}  // namespace

// Control flow of a single step of the simulation.
RealType one_step(RealType t, RealType dt,
                  std::vector<Individual<PVivax>>& population,
                  const Parameters& params, RealType eir) {
  // We need the value for omega within this timestep, however, it requires
  // knowing information over the whole population. As such we calculate these
  // details during the previous timestep. It is not guaranteed that there has
  // been a previous timestep, so we have used an optional to determine when to
  // update the cache.
  const auto cache_is_empty = !kcached_data.has_value();
  const auto cache_needs_calculation =
      cache_is_empty || kcached_data.value().isCacheInvalid(t);
  if (cache_needs_calculation) {
    // Recalculate the cache when it is out of date - currently determined by
    // time only.
    calculateRequiredCache(population, params.rho, params.age_0, t);
  }

  // Get biting parameters to calculate Lambda
  const auto rho = params.rho, age_0 = params.age_0;
  const auto omega = kcached_data.value().omega_;

  // Variables to be calculated during loop over individuals
  auto cacheable_omega = 0.0;
  // auto foi_human_to_mosquito = 0.0;
  std::vector<MaternalImmunity> maternal_immunity_store;
  maternal_immunity_store.reserve(population.size() / 4);

  // eir is calculable from the mosquito state - or retrievable from the
  // cache if this becomes too much of a calculation
  // Update the state of each individual one by one - cache any values that will
  // be of use in the next time step - we should determine if replace_if can be
  // used here instead - this may minimise copying.
  const auto pop_size = population.size();
  const auto erase_it = std::remove_if(
      population.begin(), population.end(),
      [&](Individual<PVivax>& person) -> bool {
        // Log the current status of the individual for output.

        // One step the individual - but they must not exceed maximum age.
        auto& state = person.status_;
        auto& age = person.age_;
        const auto age_exceeds_max_age = age > params.max_age;
        if (age_exceeds_max_age) {
          return kEXIT_INDIVIDUAL_DIES;
        }

        const auto X = state.getOmega() / omega, zeta = state.getZeta(),
                   lambda = eir * X * zeta;
        const auto [isIndividualDead, c] =
            UpdateState(state, params, lambda, t, dt);

        // Update human to mosquito FOI
        // foi_human_to_mosquito += zeta * X * c;

        // Does the individual contribute to maternal immunity - we do not have
        // to worry about a person dying after we deem that they are capable of
        // giving birth. We use a different maternity store to make sure there
        // is no bad references.
        const auto individual_is_birthing_capable = false;
        if (individual_is_birthing_capable) {
          // Track the immunity levels of all birthing capable individuals to
          // sample from at the time of birth.
          maternal_immunity_store.emplace_back(state.getParasiteImmunity(),
                                               state.getClinicalImmunity());
        }

        // The individual is dead they cannot contribute from here on out
        // if an individual is to die this timestep than they cannot be a
        // birthing
        // person on the next time step, nor can their replacement (they will be
        // aged zero)
        if (isIndividualDead) {
          return isIndividualDead;
        }

        // Update all details in individual that is not status - includes
        // immunity - do we want to update the 1.0 - rho etc when we update age
        // and possibly cache it within the individual. This may allow
        // replace_if to run faster - oh there is a bug here. cacheable_omega
        // should be calculated with the new individuals.
        age += dt;
        // Calculate an individual level omega to be within the person for the
        // next timestep - when this individual is constructed they should be
        // stored too.
        const auto omega_individual = 1.0 - rho * std::exp(-age / age_0);
        state.setOmega(omega_individual);
        cacheable_omega += omega_individual;
        state.updateImmunity(t, params.exp_rate_dt_parasite_immunity,
                             params.exp_rate_dt_clinical_immunity,
                             params.exp_rate_dt_maternal_immunity, age,
                             params.end_maternal_immunity);
        return isIndividualDead;
      });
  population.erase(erase_it, population.end());

  // Birth an equal amount of individuals to those that died.
  // We need to randomly determine who the new individual's birthing person is.
  // If the maternal_immunity_store is empty, we add zero to it. This is the
  // edge case of no birthing persons being present, which whilst unphysical,
  // may sometimes occur within the code - especially during the burn in phase
  // if we have to initialise a large amount of individuals at 0 years of age.
  if (maternal_immunity_store.empty()) {
    maternal_immunity_store.emplace_back(0.0, 0.0);
  }
  auto gen_birthing_person = std::uniform_int_distribution<SizeType>(
      0, maternal_immunity_store.size() - 1);
  auto gen_zeta = std::lognormal_distribution<RealType>(
      params.biting_rate_log_mean, params.biting_rate_log_sd);
  while (population.size() != pop_size) {
    // Sample the birthing persons immunity - we use the unsafe operator[] for
    // access to the underlying type as we are guaranteed it will be accessible
    // due to previous lines of code.
    const auto birthing_person_ref = gen_birthing_person(generator);
    const auto& [parasite_immunity, clinical_immunity] =
        maternal_immunity_store[birthing_person_ref];

    // Create the new individual with some proportion of their birthing persons
    // immunity
    const auto zeta = gen_zeta(generator);
    population.emplace_back(
        0.0, Status::S, params.proportion_maternal_immunity * parasite_immunity,
        params.proportion_maternal_immunity * clinical_immunity, zeta, rho,
        age_0);
    cacheable_omega += population.back().status_.getOmega();
  }

  // Time output.
  const auto tout = t + dt;

  // Store cached values and data required to confirm validity of the cache
  kcached_data.emplace(cacheable_omega, tout);

  return tout;
};
}  // namespace white
}  // namespace vivax
}  // namespace plasx