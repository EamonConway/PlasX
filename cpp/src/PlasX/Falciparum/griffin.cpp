#include "PlasX/Falciparum/griffin.hpp"

#include <algorithm>
#include <iostream>

#include "PlasX/random.hpp"
namespace plasx {
namespace falciparum {
namespace griffin {

// Delay between bite and infection.
auto delay = 0.0;

static bool determine_event(double lambda, double dt) {
  // Determine if someone is infected.
  auto r = genunf_std(generator);
  if (exp(-dt * lambda) < r) {
    return true;  // You were infected.
  }
  return false;  // You were not infected.
}

static void SAU_infection(PFalc& state, const Parameters& params,
                          const double t) {
  // This function determines what happens with an infection in the S A or U
  // compartment. It is assumed that Treatment wipes all infections that could
  // occur

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
    state.current_ = Status::A;
    return;
  }

  // Clinical infections - treated or untreated.
  auto is_treated = r2 <= f_T;
  if (is_treated) {
    state.clearInfectionQueue();
    state.current_ = Status::T;
  } else {
    // You have an untreated clinical disease
    state.current_ = Status::D;
  }
  return;
}

// Update the state of individuals.
static bool S_update(PFalc& state, const Parameters& params,
                     const double lambda, const double t, const double dt) {
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
    return death;
  }

  // There was an infection activated, determine what happened.
  SAU_infection(state, params, t);
  return false;
}

static bool A_update(PFalc& state, const Parameters& params,
                     const double lambda, const double t,
                     const double dt) noexcept {
  // Construct the rate that the individual will leave A .
  const auto r_A0 = params.r_A0, kappa_A = params.kappa_A, I_A0 = params.I_A0,
             w_A = params.w_A, mu_d = params.mu_d;
  const auto IA_ratio_power_inverse = pow(state.getIA() / I_A0, -kappa_A);
  // Reformulate to be more stable. If the top and bottom got too large youd
  // be in trouble.
  const auto r_A = r_A0 * (1.0 + (w_A - 1.0) / (1.0 + IA_ratio_power_inverse));

  // Either something will happen, or nothing will happen.
  const auto prob_event = r_A + mu_d;

  // Check to see if a bite occurs this time step.
  auto successful_bite = determine_event(lambda, dt);
  if (successful_bite) {
    // Add this infection to the schedule with the appropriate delay.
    state.scheduleInfection(t + delay);
  }

  // Check and update the infection Queue - this function changes the update
  // function.
  auto infection_active = state.updateInfection(t);
  if (infection_active) {
    SAU_infection(state, params, t);
    return false;
  }

  // Does a non-infection event occur.
  const auto event_occurs = determine_event(prob_event, dt);
  if (!event_occurs) {
    return false;
  }

  // What event occurs.
  const auto r = genunf_std(generator);
  const auto death = r < mu_d / prob_event;
  if (!death) {
    // Move from A to U.
    state.current_ = Status::U;
  }
  return death;
}

static bool U_update(PFalc& state, const Parameters& params,
                     const double lambda, const double t,
                     const double dt) noexcept {
  // In this compartment you can be infected or move to susceptible.
  const auto mu_d = params.mu_d;
  const auto prob_event = params.r_U + mu_d;
  // Check to see if a bite occurs this time step.
  auto successful_bite = determine_event(lambda, dt);
  if (successful_bite) {
    // Add this infection to the schedule with the appropriate delay.
    state.scheduleInfection(t + delay);
  }

  // Check and update the infection Queue - this function changes the update
  // function.
  auto infection_active = state.updateInfection(t);
  if (infection_active) {
    SAU_infection(state, params, t);
    return false;
  }

  // Does a non-infection event occur.
  const auto event_occurs = determine_event(prob_event, dt);
  if (!event_occurs) {
    return false;
  }

  // Hey something is going to happen, but what! Lets find out.
  const auto r = genunf_std(generator);  // random number
  const auto death = r < mu_d / prob_event;
  if (!death) {
    // Move to S
    state.current_ = Status::S;
  }
  return death;
}

static bool D_update(PFalc& state, const Parameters& params,
                     const double lambda, const double t,
                     const double dt) noexcept {
  // This checks to see if the time you are in D is enough to transition.
  const auto mu_d = params.mu_d;
  const auto prob_event = params.r_D + mu_d;

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
  auto infection_active = state.updateInfection(t);
  if (infection_active) {
    // They go to D... so do not remove them from D and continue to do nothing
    // else.
    return false;
  }

  // Does a non-infection event occur.
  const auto event_occurs = determine_event(prob_event, dt);
  if (!event_occurs) {
    return event_occurs;
  }

  const auto r = genunf_std(generator);  // random number
  const auto death = r < mu_d / prob_event;
  if (!death) {
    // You've been here long enough, move from D to A.
    state.current_ = Status::A;
  }
  return death;
}

static bool T_update(PFalc& state, const Parameters& params,
                     const double lambda, const double t, const double dt) {
  // This checks to see if the time you are in T is enough to transition.
  const auto mu_d = params.mu_d;
  const auto prob_event = params.r_T + mu_d;
  const auto event_occurs = determine_event(prob_event, dt);

  if (!event_occurs) {
    return event_occurs;
  }

  const auto r = genunf_std(generator);
  const auto death = r < mu_d / prob_event;
  if (!death) {
    state.current_ = Status::P;
  }
  return death;
}

static bool P_update(PFalc& state, const Parameters& params,
                     const double lambda, const double t, double dt) {
  // This checks to see if the time you are in P is enough to transition.
  const auto mu_d = params.mu_d;
  const auto prob_event = params.r_P + mu_d;
  const auto event_occurs = determine_event(prob_event, dt);

  if (!event_occurs) {
    return event_occurs;
  }

  const auto r = genunf_std(generator);
  const auto death = r < mu_d / prob_event;
  if (!death) {
    state.current_ = Status::S;
  }
  return death;
}

RealType one_step(const double t, const double dt,
                  std::vector<Individual<PFalc>>& population,
                  const Parameters& params, double eir) {
  // Get biting parameters to calculate Lambda
  const auto b_min = params.b_min, b_max = params.b_max, I_B0 = params.I_B0,
             kappa_B = params.kappa_B, rho = params.rho, age_0 = params.age_0,
             bdiff = b_max - b_min;

  // Force of infection from people to mosquito - must be calculated and passed
  // on.
  // auto foi_mosquito = 0.0;

  // Loop over individuals
  auto erase_it = std::remove_if(
      population.begin(), population.end(),
      [&](Individual<PFalc>& person) -> bool {
        auto& state = person.status_;
        auto& age = person.age_;
        // Construct Lambda(t) for each individual.
        auto b = b_min + bdiff / (1.0 + pow(state.getIB() / I_B0, kappa_B));
        auto psi = 1.0 - rho * std::exp(-age / age_0);
        auto zeta = state.getZeta();
        // It is plausible to add this to the individual for use when it
        // comes to calculating the normalization constant etc in the
        // mosquito model.
        auto lambda = eir * psi * b * zeta;

        switch (state.current_) {
          case Status::S:
            return S_update(state, params, lambda, t, dt);
            break;
          case Status::A:
            return A_update(state, params, lambda, t, dt);
            break;
          case Status::U:
            return U_update(state, params, lambda, t, dt);
            break;
          case Status::D:
            return D_update(state, params, lambda, t, dt);
            break;
          case Status::T:
            return T_update(state, params, lambda, t, dt);
            break;
          case Status::P:
            return P_update(state, params, lambda, t, dt);
            break;
          default:
            throw std::logic_error("You messed up");
        }
      });
  population.erase(erase_it, population.end());

  std::cout << "Pop size: " << population.size() << std::endl;
  return t + dt;
}

// Construct the object that will store the information in the Griffin
// simulation.
PFalc::PFalc(const Status& status, double ICA, double ICM, double IA)
    : current_(status),
      I_CA_(ICA),
      I_CM_(ICM),
      I_A_(IA),
      zeta_(1.0),
      I_B_(0.0) {
  auto infty = std::numeric_limits<double>::infinity();
  infection_queue_ =
      std::priority_queue<double, std::vector<double>, std::greater<double>>();
  infection_queue_.push(infty);
};

double PFalc::getIC() noexcept { return I_CA_ + I_CM_; }

double PFalc::getIA() noexcept { return I_A_; }

void PFalc::clearInfectionQueue() noexcept {
  // Replace with a queue consisting only of an infection at infinity.
  while (infection_queue_.size() != 1) {
    infection_queue_.pop();
  }
}

void PFalc::scheduleInfection(const double t) { infection_queue_.push(t); };

bool PFalc::updateInfection(const double t) {
  // Technically, by using a priority queue there is an indirection to the
  // stack as all data is held in a vector. We could consider caching the top
  // value in the individual structure and only go away once that value is
  // outdated. If the infectionQueue is empty, the next infection could be at
  // time infinity. We would have to make sure that we update things
  // appropriately for that however. This may not be the best, at a new
  // infection we would have to check against cache value. If cahced value is
  // larger, we need to move it bak into the queue. But this might just be fine.
  // It is an interesting thing to look for.

  auto activate_infection = t >= infection_queue_.top();
  if (!activate_infection) {
    return false;
  }

  while (t >= infection_queue_.top()) {
    infection_queue_.pop();
  }
  return true;
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

}  // namespace griffin
}  // namespace falciparum
}  // namespace plasx
