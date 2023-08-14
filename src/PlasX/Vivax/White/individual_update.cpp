#include "PlasX/Vivax/White/individual_update.hpp"

#include "PlasX/random.hpp"
namespace plasx {
namespace vivax {
namespace white {
// Unnamed namespace for all functions with internal linkage.
namespace {
// Constant values for return types. Make explicit what should be returned from
// functions when an individual lives or dies. The prefix k is used to signify
// that it is global within this translation unit.
constexpr bool kEXIT_INDIVIDUAL_LIVES = false;
constexpr bool kEXIT_INDIVIDUAL_DIES = true;

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
  // Activate the treatment pathway for the individual - if you do not want to
  // be passing functions around, then this should be set somehow in a global
  // sense.
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
             mu_d = params.mu_d, delay = params.delay;

  // Check to see if a bite or relapse occurs this time step - causing an
  // infection after some delay.
  const auto prob_infection = lambda + kf;
  const auto queue_new_infection = genunf_std(generator) < prob_infection * dt;
  if (queue_new_infection) {
    state.queueInfection(t, lambda, prob_infection, delay);
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

  // Clearance or death will occur.
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
             mu_d = params.mu_d, delay = params.delay;

  // Check to see if a bite or relapse occurs this time step - causing an
  // infection after some delay.
  const auto prob_infection = lambda + kf;
  const auto queue_new_infection = genunf_std(generator) < prob_infection * dt;
  if (queue_new_infection) {
    state.queueInfection(t, lambda, prob_infection, delay);
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
             mu_d = params.mu_d, r_LM = params.r_LM, delay = params.delay;

  // Check to see if a bite or relapse occurs this time step - causing an
  // infection after some delay.
  const auto prob_infection = lambda + kf;
  const auto queue_new_infection = genunf_std(generator) < prob_infection * dt;
  if (queue_new_infection) {
    state.queueInfection(t, lambda, prob_infection, delay);
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
             mu_d = params.mu_d, r_D = params.r_D, delay = params.delay;

  // Check to see if a bite or relapse occurs this time step - causing an
  // infection after some delay.
  const auto prob_infection = lambda + kf;
  const auto queue_new_infection = genunf_std(generator) < prob_infection * dt;
  if (queue_new_infection) {
    state.queueInfection(t, lambda, prob_infection, delay);
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
             mu_d = params.mu_d, r_T = params.r_T, delay = params.delay;

  // Check to see if a bite or relapse occurs this time step - causing an
  // infection after some delay.
  const auto prob_infection = lambda + kf;
  const auto queue_new_infection = genunf_std(generator) < prob_infection * dt;
  if (queue_new_infection) {
    state.queueInfection(t, lambda, prob_infection, delay);
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
             mu_d = params.mu_d, r_P = params.r_P, delay = params.delay;

  // Check to see if a bite or relapse occurs this time step - causing an
  // infection after some delay.
  const auto prob_infection = lambda + kf;
  const auto queue_new_infection = genunf_std(generator) < prob_infection * dt;
  if (queue_new_infection) {
    state.queueInfection(t, lambda, prob_infection, delay);
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

static IndividualOneStepReturnType UpdateState(PVivax& state,
                                               const Parameters& params,
                                               const RealType lambda,
                                               const RealType t,
                                               const RealType dt) {
  auto output = IndividualOneStepReturnType{false, 0.0};
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

IndividualOneStepReturnType IndividualOneStep(const RealType t,
                                              const RealType dt,
                                              Individual<PVivax>& person,
                                              const Parameters& params,
                                              const RealType eir_omega) {
  // One step the individual - but they must not exceed maximum age.
  auto& state = person.status_;
  auto& age = person.age_;

  // Early return if you are older than max age
  const auto age_exceeds_max_age = age >= params.max_age;
  if (age_exceeds_max_age) {
    return {true, 0.0};
  }

  // Calculate the individual level force of infection and update state.
  const auto individual_omega = state.getOmega(), zeta = state.getZeta(),
             lambda = eir_omega * individual_omega * zeta;
  const auto state_output = UpdateState(state, params, lambda, t, dt);

  // Update remaining details - we are also updating individuals that will die.
  age += dt;
  state.setOmega(1.0 - params.rho * std::exp(-age / params.age_0));
  state.updateImmunity(t, params.exp_rate_dt_parasite_immunity,
                       params.exp_rate_dt_clinical_immunity,
                       params.exp_rate_dt_maternal_immunity, age,
                       params.end_maternal_immunity);
  return state_output;
};
}  // namespace white
}  // namespace vivax
}  // namespace plasx