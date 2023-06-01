#include "PlasX/Falciparum/griffin.hpp"

#include "PlasX/random.hpp"

namespace plasx {
namespace falciparum {
namespace griffin {
auto delay = 0.0;

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

RealType one_step_no_switch(const double t, const double dt,
                            std::vector<Individual<PFalc>>& population,
                            const Parameters& params, double eir,
                            std::vector<size_t>& S, std::vector<size_t>& A,
                            std::vector<size_t>& U, std::vector<size_t>& D,
                            std::vector<size_t>& T, std::vector<size_t>& P,
                            std::vector<size_t>& Dead) {
  // Get biting parameters to calculate Lambda
  auto b_min = params.b_min, b_max = params.b_max, I_B0 = params.I_B0,
       kappa_B = params.kappa_B, rho = params.rho, age_0 = params.age_0,
       bdiff = b_max - b_min;

  // Size of compartments before switching.
  const auto A_size = A.size(), D_size = D.size(), T_size = T.size();

  // Force of infection from people to mosquito - must be calculated and passed
  // on.
  // auto foi_mosquito = 0.0;

  // Loop over individuals - if you do S first, you can loop through from
  // beginning to end, then noone will have gone to the end of P, so you can
  // loop through the entirety of P, we can now loop through the entirety of U
  // as well, as noone can go there at this point too. S will have made
  // individiauls move to A D T or null, so cant loop through all of them. S.
  auto s_remove_it =
      std::remove_if(S.begin(), S.end(), [&](const int id) -> bool {
        // Important to remember that all updates to parameters must be done
        // at end of function and not at begining.

        auto& state = population[id].status_;
        const auto& age = population[id].age_;
        const auto& IB = state.getIB();
        // Construct Lambda(t) for each individual.
        auto b = b_min + bdiff / (1.0 + pow(IB / I_B0, kappa_B));
        auto psi = 1.0 - rho * std::exp(-age / age_0);

        // It is plausible to add this to the individual for use when it
        // comes to calculating the normalization constant etc in the
        // mosquito model.
        auto lambda = eir * psi * b * state.getZeta();

        // This function returns a boolean to determine if the individual leaves
        // the S compartment.
        auto isRemoved =
            S_update(id, state, params, lambda, t, dt, A, D, T, Dead);
        return isRemoved;
      });
  S.erase(s_remove_it, S.end());

  // P
  auto p_remove_it =
      std::remove_if(P.begin(), P.end(), [&](const int id) -> bool {
        auto& state = population[id].status_;
        auto& age = population[id].age_;

        // Construct Lambda(t) for each individual.
        auto b = b_min + bdiff / (1.0 + pow(state.getIB() / I_B0, kappa_B));
        auto psi = 1.0 - rho * std::exp(-age / age_0);

        // It is plausible to add this to the individual for use when it
        // comes to calculating the normalization constant etc in the
        // mosquito model.
        auto lambda = eir * psi * b * state.getZeta();

        // This function returns a boolean to determine if the individual leaves
        // the P compartment.
        auto isRemoved = P_update(id, state, params, lambda, t, dt, S, Dead);
        return isRemoved;
      });
  P.erase(p_remove_it, P.end());

  // U
  auto u_remove_it =
      std::remove_if(U.begin(), U.end(), [&](const int id) -> bool {
        auto& state = population[id].status_;
        auto& age = population[id].age_;

        // Construct Lambda(t) for each individual.
        auto b = b_min + bdiff / (1.0 + pow(state.getIB() / I_B0, kappa_B));
        auto psi = 1.0 - rho * std::exp(-age / age_0);

        // It is plausible to add this to the individual for use when it
        // comes to calculating the normalization constant etc in the
        // mosquito model.
        auto lambda = eir * psi * b * state.getZeta();

        // This function returns a boolean to determine if the individual leaves
        // the U compartment.
        auto isRemoved =
            U_update(id, state, params, lambda, t, dt, S, A, D, T, Dead);
        return isRemoved;
      });
  U.erase(u_remove_it, U.end());

  // A
  const auto A_end = A.begin() + A_size;
  auto a_remove_it =
      std::remove_if(A.begin(), A_end, [&](const int id) -> bool {
        auto& state = population[id].status_;
        auto& age = population[id].age_;

        // Construct Lambda(t) for each individual.
        auto b = b_min + bdiff / (1.0 + pow(state.getIB() / I_B0, kappa_B));
        auto psi = 1.0 - rho * std::exp(-age / age_0);

        // It is plausible to add this to the individual for use when it
        // comes to calculating the normalization constant etc in the
        // mosquito model.
        auto lambda = eir * psi * b * state.getZeta();

        // This function returns a boolean to determine if the individual leaves
        // the A compartment.
        auto isRemoved =
            A_update(id, state, params, lambda, t, dt, A, U, D, T, Dead);
        return isRemoved;
      });
  A.erase(a_remove_it, A_end);

  // D
  const auto D_end = D.begin() + D_size;
  auto d_remove_it =
      std::remove_if(D.begin(), D_end, [&](const int id) -> bool {
        auto& state = population[id].status_;
        auto& age = population[id].age_;

        // Construct Lambda(t) for each individual.
        auto b = b_min + bdiff / (1.0 + pow(state.getIB() / I_B0, kappa_B));
        auto psi = 1.0 - rho * std::exp(-age / age_0);
        auto zeta = state.getZeta();
        // It is plausible to add this to the individual for use when it
        // comes to calculating the normalization constant etc in the
        // mosquito model.
        auto lambda = eir * psi * b * zeta;

        // This function returns a boolean to determine if the individual leaves
        // the D compartment.
        auto isRemoved = D_update(id, state, params, lambda, t, dt, A, Dead);
        return isRemoved;
      });
  D.erase(d_remove_it, D_end);

  // T
  const auto T_end = T.begin() + T_size;
  auto t_remove_it =
      std::remove_if(T.begin(), T_end, [&](const int id) -> bool {
        auto& state = population[id].status_;
        auto& age = population[id].age_;

        // Construct Lambda(t) for each individual.
        auto b = b_min + bdiff / (1.0 + pow(state.getIB() / I_B0, kappa_B));
        auto psi = 1.0 - rho * std::exp(-age / age_0);

        // It is plausible to add this to the individual for use when it
        // comes to calculating the normalization constant etc in the
        // mosquito model.
        auto lambda = eir * psi * b * state.getZeta();

        // This function returns a boolean to determine if the individual leaves
        // the T compartment.
        auto isRemoved = T_update(id, state, params, lambda, t, dt, P, Dead);
        return isRemoved;
      });
  T.erase(t_remove_it, T_end);

  // Anyone that is susceptible does not contribute to mosquito infection. We
  // should take advantage of that to speed up calculation.

  // Mosquitoes can go here - use the appropriate parameters (currenly not
  // listed).

  // Birth processes, we want to fill up the Null population first.
  // auto decay_IB = person.I_B / params.d_B;
  // auto decay_IA = person.I_A / params.d_A;
  // auto decay_ICA = person.I_CA / params.d_C;
  // auto decay_ICM = person.I_CM / params.d_M;

  // person.I_B += dt * (h_function(eir) - decay_IB);
  // person.I_A += dt * (h_function(lambda) - decay_IA);
  // person.I_CA += dt * (h_function(lambda) - decay_ICA);
  // person.I_CM += dt * (-decay_ICM);

  return t + dt;
}

// RealType one_step_switch(const double t, const double dt,
//                          std::vector<Individual<PFalc>>& population,
//                          const Parameters& params, double eir) {
//   // dt - time step size.
//   // eir - entomological innoculation rate
//   // person - current person that is being updated
//   // params - Parameters required for the simulation.

//   // Loop over individuals.
//   auto end_it = std::remove_if(
//       population.begin(), population.end(),
//       [&](Individual<PFalc>& person) -> bool {
//         // Important to remember that all updates to parameters
//         // must be done at end of function and not at begining.

//         // Get biting parameters to calculate Lambda
//         auto b_min = params.b_min, b_max = params.b_max, I_B0 = params.I_B0,
//              kappa_B = params.kappa_B;

//         // Construct Lambda(t) for each individual.
//         auto b =
//             b_min + (b_max - b_min) /
//                         (1.0 + pow(person.status_.getIB() / I_B0, kappa_B));
//         auto psi = 1.0 - params.rho * std::exp(-person.age_ / params.age_0);

//         // It is plausible to add this to the individual for use when it
//         comes
//         // to calculating the normalization constant etc in the mosquito
//         model. auto lambda = eir * psi * b * person.status_.getZeta();

//         // This function returns a boolean to determine if the individual
//         will
//         // die.
//         auto death = false;
//         switch (person.status_.current_) {
//           case Status::S:
//             death = S_update(person.status_, params, lambda, t, dt);
//             break;
//           case Status::A:
//             death = A_update(person.status_, params, lambda, t, dt);
//             break;
//           case Status::U:
//             death = U_update(person.status_, params, lambda, t, dt);
//             break;
//           case Status::D:
//             death = D_update(person.status_, params, lambda, t, dt);
//             break;
//           case Status::T:
//             death = T_update(person.status_, params, lambda, t, dt);
//             break;
//           case Status::P:
//             death = P_update(person.status_, params, lambda, t, dt);
//             break;
//           default:
//             throw std::logic_error(
//                 "Incorrect compartment specified for individual in  "
//                 "one_step_switch, line number " +
//                 std::to_string(__LINE__));
//         }

//         person.age_ += dt;

//         // Update any immunity parameters.
//         // auto decay_IB = person.I_B / params.d_B;
//         // auto decay_IA = person.I_A / params.d_A;
//         // auto decay_ICA = person.I_CA / params.d_C;
//         // auto decay_ICM = person.I_CM / params.d_M;

//         // person.I_B += dt * (h_function(eir) - decay_IB);
//         // person.I_A += dt * (h_function(lambda) - decay_IA);
//         // person.I_CA += dt * (h_function(lambda) - decay_ICA);
//         // person.I_CM += dt * (-decay_ICM);

//         return death;
//       });
//   population.erase(end_it, population.end());

//   return t + dt;
// }

// Construct the object that will store the information in the Griffin
// simulation.
PFalc::PFalc(const Status& status, double ICA, double ICM, double IA)
    : current_(status),
      I_CA_(ICA),
      I_CM_(ICM),
      I_A_(IA),
      zeta_(0.0),
      I_B_(0.0){
          // cached_infection_(std::numeric_limits<double>::infinity()) {
      };

double PFalc::getIC() noexcept { return I_CA_ + I_CM_; }

double PFalc::getIA() noexcept { return I_A_; }

void PFalc::clearInfectionQueue() noexcept {
  // Replace with an empty queue.
  infection_queue_ =
      std::priority_queue<double, std::vector<double>, std::less<double>>();
  cached_infection_ = std::numeric_limits<double>::infinity();
}

void PFalc::scheduleInfection(const double t) {
  const auto isCacheable = t < cached_infection_;
  // You have to do the push first. or you might overright the cached value and
  // lose it
  infection_queue_.push(!isCacheable * t + isCacheable * cached_infection_);
  cached_infection_ = isCacheable * t + !isCacheable * cached_infection_;
  // Arguably faster than having the if statements.... will have to check.
};

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
  if (t >= cached_infection_) [[unlikely]] {
    // The cached infection is going to take place.
    if (infection_queue_.empty()) {
      cached_infection_ = std::numeric_limits<double>::infinity();
    } else {
      cached_infection_ = infection_queue_.top();
      infection_queue_.pop();
    }
    return true;
  } else [[likely]] {
    return false;
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

}  // namespace griffin
}  // namespace falciparum
}  // namespace plasx