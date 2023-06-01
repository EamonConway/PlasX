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

                          std::vector<size_t>& D, std::vector<size_t>& T) {
  // This function determines what happens with an infection in the S A or U
  // compartment. I need to determine which infections come here. Infections
  // from S A and U will go into this function, as all three must go through the
  // check of phi. You can be untreated (D) and then reinfected, but you will go
  // to D again. T and P can not be re-infected, as Treatment wipes all
  // infections that could occur, and prophylaxis is killing everything off that
  // could infect you.

  // How should we handle the fact that if you are in D, you stay in D.
  // if(infection){ just stay at d}

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
  if (r1 <= phi) {
    // You have succeeded to go to the node that checks if you go to T or D.
    if (r2 <= f_T) {
      // You have been treated succesfully

      // Remove the list of future infections - treatment flushes the
      // parasite in all stages?
      state.clearInfectionQueue();

      // Update state of individual.
      state.current_ = Status::T;
      state.update_ = [&](const double lambda, const double t,
                          const double dt) -> bool {
        return T_update(state, params, lambda, t, dt);
      };
    } else {
      // You have an untreated clinical disease
      state.current_ = Status::D;
      state.update_ = [&](const double lambda, const double t,
                          const double dt) -> bool {
        return D_update(state, params, lambda, t, dt);
      };
    }
  } else {
    // You failed to go to the node that checks if you go to T or D, therefore
    // you go to A.
    state.current_ = Status::A;
    state.update_ = [&](const double lambda, const double t,
                        const double dt) -> bool {
      return A_update(state, params, lambda, t, dt);
    };
  }
}

// Interesting concept. We update infections, but you cant recover at the or
// do anything else. But you can possibly be infected (because it will go into
// the later step) Instead of checking if an infection occurs with a lagged
// force of infection. We will schedule the appropriate time for that to
// occur. You will either, have this schedule activate, in which case you can
// not trigger the other event, or all possible events can occur (infection
// will be schedule and the rest). However, that doesnt make sense, we never
// checked back to see if someone recovered. So.... we shall always check if a
// future infection is scheduled. We then check if any of the scheduled events
// occur, and if so you can not do anything else. I think that this is
// equivalent to the way that the code used to be implemented.

// Define update Functions.
static bool S_update(PFalc& state, const Parameters& params,
                     const double lambda, const double t,
                     const double dt) noexcept {
  // What do we do for a susceptible person.
  // They will either be infected or do nothing.
  // Do you get infected?
  // const auto prob_event = params.mu_d + lambda;
  // Death or infection....

  // Check to see if we schedule an infection for a later time.
  if (determine_event(lambda, dt)) {
    // Add this infection to the schedule with the appropriate delay.
    state.scheduleInfection(t + delay);
  }

  // Check if an infection needs to happen this timestep. Update the infection
  // Queue - this function changes the update_ function within state.
  if (state.updateInfection(t)) {
    // The individual has now got a new infection active - do the new infection
    // stuff.
    SAU_infection(state, params, t);
  } else if (determine_event(params.mu_d, dt)) {
    // Oh no - they die. This is handled seperately from determining if we
    // schedule an infection. This keeps the current implementation consistent
    // with the past implementation.
    return true;
  }
  // Nothing fun happened... better luck next time.
  return false;  // Nothing happened oh no.
}

static bool A_update(PFalc& state, const Parameters& params,
                     const double lambda, const double t,
                     const double dt) noexcept {
  // In this compartment you can be infected or move from A into U with

  // Construct the rate that the individual will leave A .
  const auto r_A0 = params.r_A0, kappa_A = params.kappa_A, I_A0 = params.I_A0,
             w_A = params.w_A;
  const auto IA_ratio_power_inverse = pow(state.getIA() / I_A0, -kappa_A);
  // Reformulate to be more stable. If the top and bottom got too large youd
  // be in trouble.
  const auto r_A = r_A0 * (1.0 + (w_A - 1.0) / (1.0 + IA_ratio_power_inverse));

  // Either something will happen, or nothing will happen.
  const auto prob_event = r_A + params.mu_d;

  // Do we want to schedule an infection?
  if (determine_event(lambda, dt)) {
    // Schedule an infection
    state.scheduleInfection(t + delay);
  }

  // Check and update the infection Queue - this function changes the update
  // function.
  if (state.updateInfection(t)) {
    SAU_infection(state, params, t);
  } else if (determine_event(prob_event, dt)) {
    // Hey something is going to happen, but what! Lets find out.
    const auto r = genunf_std(generator);  // random number
    if (r < r_A / prob_event) {
      // You've recovered instead, move from A to U.
      state.current_ = Status::U;
      state.update_ = [&](const double lambda, const double t,
                          const double dt) -> bool {
        return U_update(state, params, lambda, t, dt);
      };
    } else {
      return true;  // Death
    }
  }

  return false;
}

static bool U_update(PFalc& state, const Parameters& params,
                     const double lambda, const double t,
                     const double dt) noexcept {
  // In this compartment you can be infected or move to susceptible.
  const auto prob_event = params.r_U + params.mu_d;

  // Do we want to schedule an infection?
  if (determine_event(lambda, dt)) {
    // Schedule an infection
    state.scheduleInfection(t + delay);
  }

  // Check and update the infection Queue - this function changes the update
  // function.
  if (state.updateInfection(t)) {
    SAU_infection(state, params, t);
  } else if (determine_event(prob_event, dt)) {
    // Hey something is going to happen, but what! Lets find out.
    const auto r = genunf_std(generator);  // random number
    if (r < params.r_U / prob_event) {
      state.current_ = Status::S;
      state.update_ = [&](const double lambda, const double t,
                          const double dt) -> bool {
        return S_update(state, params, lambda, t, dt);
      };
    } else {
      // Oh no death.
      return true;
    }
  }
  return false;
}

static bool D_update(PFalc& state, const Parameters& params,
                     const double lambda, const double t,
                     const double dt) noexcept {
  // This checks to see if the time you are in D is enough to transition.
  const auto prob_event = params.r_D + params.mu_d;

  // Do we want to schedule an infection?
  if (determine_event(lambda, dt)) {
    // Schedule an infection
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
    // They go to D... so its still D update? Do nothing? Will have to check.
  } else if (determine_event(prob_event, dt)) {
    // Hey something is going to happen, but what! Lets find out.
    const auto r = genunf_std(generator);  // random number
    if (r < params.r_D / prob_event) {
      // You've been here long enough, move from D to A.
      state.current_ = Status::A;
      state.update_ = [&](double lambda, const double t, double dt) -> bool {
        return A_update(state, params, lambda, t, dt);
      };
    } else {
      // Oh no death.
      return true;
    }
  }
  return false;
}

static bool T_update(PFalc& state, const Parameters& params,
                     const double lambda, const double t,
                     const double dt) noexcept {
  // This checks to see if the time you are in T is enough to transition.
  const auto prob_event = params.r_T + params.mu_d;
  if (determine_event(prob_event, dt)) {
    // Hey something is going to happen, but what! Lets find out.
    const auto r = genunf_std(generator);  // random number

    if (r < params.r_T / prob_event) {
      // You've been here long enough, move from T to P.
      state.current_ = Status::P;
      state.update_ = [&](const double lambda, const double t,
                          const double dt) -> bool {
        return P_update(state, params, lambda, t, dt);
      };
    } else {
      // Oh no, you died.
      return true;
    }
  }

  return false;
}

static bool P_update(PFalc& state, const Parameters& params,
                     const double lambda, const double t, double dt) noexcept {
  // This checks to see if the time you are in P is enough to transition.
  const auto prob_event = params.r_P + params.mu_d;
  if (determine_event(prob_event, dt)) {
    // Hey something is going to happen, but what! Lets find out.
    const auto r = genunf_std(generator);
    if (r < params.r_P / prob_event) {
      // You've been here long enough, move from P to S.
      state.current_ = Status::S;
      state.update_ = [&](const double lambda, const double t,
                          const double dt) -> bool {
        return S_update(state, params, lambda, t, dt);
      };
    } else {
      // Oh no, you died.
      return true;
    }
  }
  return false;
}
}  // namespace griffin
}  // namespace falciparum
}  // namespace plasx