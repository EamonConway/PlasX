#include "PlasX/Falciparum/griffin.hpp"

#include "PlasX/random.hpp"

namespace plasx {
namespace falciparum {
namespace griffin {

static bool determine_event(double lambda, double dt) {
  // Determine if someone is infected.
  auto r = genunf_std(generator);
  if (exp(-dt * lambda) < r) {
    return true;  // You were infected.
  }
  return false;  // You were not infected.
}

static bool SAU_infection(const int id, PFalc& state, const Parameters& params,
                          const double t, std::vector<size_t>& A,
                          std::vector<size_t>& D, std::vector<size_t>& T) {
  // This function determines what happens with an infection in the S A or U
  // compartment. I need to determine which infections come here. Infections
  // from S A and U will go into this function, as all three must go through the
  // check of phi. You can be untreated (D) and then reinfected, but you will go
  // to D again. T and P can not be re-infected, as Treatment wipes all
  // infections that could occur, and prophylaxis is killing everything off that
  // could infect you.

  // You only come into this function if you are in S A or U, we do not need to
  // consider the case of D going to D.
  const auto r1 = genunf_std(generator), r2 = genunf_std(generator);

  // Get parameters
  const auto f_T = params.f_T;  // Is this a constant?
  const auto I_C0 = params.I_C0;
  const auto kappa_C = params.kappa_C;
  // Do not have this in the individual as we do not want accidentally forget
  // to update it.
  const auto I_C = state.getIC();
  // Get parameters for phi (immunity dependent)
  const auto IC_ratio = I_C / I_C0;
  const auto phi = 1.0 / (1.0 + pow(IC_ratio, kappa_C));

  // Which compartment does the new infection go to.
  auto clinical_infection = r1 <= phi;
  if (!clinical_infection) {
    A.emplace_back(id);
    state.current_ = Status::A;
    return true;
  }

  // Clinical infections - treated or untreated.
  auto is_treated = r2 <= f_T;
  if (is_treated) {
    state.clearInfectionQueue();
    T.emplace_back(id);
    state.current_ = Status::T;
  } else {
    // You have an untreated clinical disease
    D.emplace_back(id);
    state.current_ = Status::D;
  }
  return true;
}

// Update the state of individuals.
static bool S_update(const int id, PFalc& state, const Parameters& params,
                     const double lambda, const double t, const double dt,
                     std::vector<size_t>& A, std::vector<size_t>& D,
                     std::vector<size_t>& T, std::vector<size_t>& Dead) {
  // Check to see if a bite occurs this time step.
  auto successful_bite = determine_event(lambda, dt);
  if (successful_bite) {
    // Add this infection to the schedule with the appropriate delay.
    state.scheduleInfection(t + delay);
  }

  // Check if a prior bite becomes an active infection this timestep.
  auto infection_active = state.updateInfection(t);
  if (!infection_active) {
    auto death = determine_event(params.mu_d, dt);
    if (death) {
      Dead.emplace_back(id);
    }
    return death;
  }

  return SAU_infection(id, state, params, t, A, D, T);
}

static bool A_update(const int id, PFalc& state, const Parameters& params,
                     const double lambda, const double t, const double dt,
                     std::vector<size_t>& A, std::vector<size_t>& U,
                     std::vector<size_t>& D, std::vector<size_t>& T,
                     std::vector<size_t>& Dead) noexcept {
  // Construct the rate that the individual will leave A .
  const auto r_A0 = params.r_A0, kappa_A = params.kappa_A, I_A0 = params.I_A0,
             w_A = params.w_A;
  const auto IA_ratio_power_inverse = pow(state.getIA() / I_A0, -kappa_A);
  // Reformulate to be more stable. If the top and bottom got too large youd
  // be in trouble.
  const auto r_A = r_A0 * (1.0 + (w_A - 1.0) / (1.0 + IA_ratio_power_inverse));

  // Either something will happen, or nothing will happen.
  const auto prob_event = r_A + params.mu_d;

  // Check to see if a bite occurs this time step.
  auto successful_bite = determine_event(lambda, dt);
  if (successful_bite) {
    // Add this infection to the schedule with the appropriate delay.
    state.scheduleInfection(t + delay);
  }

  // Check and update the infection Queue - this function changes the update
  // function.
  if (state.updateInfection(t)) {
    return SAU_infection(id, state, params, t, A, D, T);
  } else if (determine_event(prob_event, dt)) {
    // Hey something is going to happen, but what! Lets find out.
    const auto r = genunf_std(generator);  // random number
    if (r < r_A / prob_event) {
      // You've recovered instead, move from A to U.
      U.emplace_back(id);
      state.current_ = Status::U;
    } else {
      return true;  // Death
    }
  }

  return false;
}

static bool U_update(const int id, PFalc& state, const Parameters& params,
                     const double lambda, const double t, const double dt,
                     std::vector<size_t>& S, std::vector<size_t>& A,
                     std::vector<size_t>& D, std::vector<size_t>& T,
                     std::vector<size_t>& Dead) noexcept {
  // In this compartment you can be infected or move to susceptible.
  const auto prob_event = params.r_U + params.mu_d;
  // Check to see if a bite occurs this time step.
  auto successful_bite = determine_event(lambda, dt);
  if (successful_bite) {
    // Add this infection to the schedule with the appropriate delay.
    state.scheduleInfection(t + delay);
  }

  // Check and update the infection Queue - this function changes the update
  // function.
  if (state.updateInfection(t)) {
    return SAU_infection(id, state, params, t, A, D, T);
  } else if (determine_event(prob_event, dt)) {
    // Hey something is going to happen, but what! Lets find out.
    const auto r = genunf_std(generator);  // random number
    if (r < params.r_U / prob_event) {
      S.emplace_back(id);
      state.current_ = Status::S;
    } else {
      // Oh no death.
      return true;
    }
  }
  return false;
}

static bool D_update(const int id, PFalc& state, const Parameters& params,
                     const double lambda, const double t, const double dt,
                     std::vector<size_t>& A,
                     std::vector<size_t>& Dead) noexcept {
  // This checks to see if the time you are in D is enough to transition.
  const auto prob_event = params.r_D + params.mu_d;

  // Check to see if a bite occurs this time step.
  auto successful_bite = determine_event(lambda, dt);
  if (successful_bite) {
    // Add this infection to the schedule with the appropriate delay.
    state.scheduleInfection(t + delay);
  }

  // Check and update the infection Queue - this function changes the update
  // function. Throw away result.

  // Do we want to skip the function if this is the case?
  // We need to keep checking the queue, but we throw out any that activate as
  // we will stay in D. Hilariously, this is equivalent to just being infected
  // and going to D again thanks to the wonders of the exponential
  // distribution.
  if (state.updateInfection(t)) {
    // They go to D... so do not remove them from D and continue to do nothing
    // else.
  } else if (determine_event(prob_event, dt)) {
    // Hey something is going to happen, but what! Lets find out.
    const auto r = genunf_std(generator);  // random number
    if (r < params.r_D / prob_event) {
      // You've been here long enough, move from D to A.
      A.emplace_back(id);
      state.current_ = Status::A;
    } else {
      // Oh no death.
      return true;
    }
  }
  return false;
}

static bool T_update(const int id, PFalc& state, const Parameters& params,
                     const double lambda, const double t, const double dt,
                     std::vector<size_t>& P, std::vector<size_t>& Dead) {
  // This checks to see if the time you are in T is enough to transition.
  const auto prob_event = params.r_T + params.mu_d;
  if (!determine_event(prob_event, dt)) {
    return false;
  }

  const auto r = genunf_std(generator);
  if (r < params.r_T / prob_event) {
    // You've been here long enough, move from T to P.
    P.emplace_back(id);
    state.current_ = Status::P;
  } else {
    Dead.emplace_back(id);
  }
  return true;
}

static bool P_update(const int id, PFalc& state, const Parameters& params,
                     const double lambda, const double t, double dt,
                     std::vector<size_t>& S, std::vector<size_t>& Dead) {
  // This checks to see if the time you are in P is enough to transition.
  const auto prob_event = params.r_P + params.mu_d;
  if (determine_event(prob_event, dt)) {
    // Hey something is going to happen, but what! Lets find out.
    const auto r = genunf_std(generator);
    if (r < params.r_P / prob_event) {
      // You've been here long enough, move from P to S.
      S.emplace_back(id);
      state.current_ = Status::S;

    } else {
      // Oh no, you died.
      Dead.emplace_back(id);
    }
    return true;
  }
  return false;
}

  }
  return false;
}
}  // namespace griffin
}  // namespace falciparum
}  // namespace plasx