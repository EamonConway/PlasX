#include "PlasX/Vivax/White/pvivax.hpp"

#include <cmath>
#include <iostream>

#include "PlasX/random.hpp"
#include "PlasX/udl.hpp"
namespace plasx {
namespace vivax {
namespace white {

std::ostream& operator<<(std::ostream& os, const PVivax& data) {
  os << data.zeta_ << ", ";
  os << data.parasite_immunity_ << ", ";
  os << data.clinical_immunity_ << ", ";
  os << data.maternal_parasite_immunity_ << ", ";
  os << data.maternal_clinical_immunity_ << ", ";
  os << data.num_hypnozoites_ << ", ";
  os << data.current_;
  return os;
}

// Birth Constructor.
PVivax::PVivax(const RealType age, const Status& status,
               const RealType maternal_parasite_immunity,
               const RealType maternal_clinical_immunity, const RealType zeta,
               const RealType rho, const RealType age_0)
    : PVivax(age, status, 0.0, 0.0, maternal_parasite_immunity,
             maternal_clinical_immunity, zeta, rho, age_0, 0){};

// Typical Constructor
PVivax::PVivax(const RealType age, const Status& status,
               const RealType parasite_immunity,
               const RealType clinical_immunity,
               const RealType maternal_parasite_immunity,
               const RealType maternal_clinical_immunity, const RealType zeta,
               const RealType rho, const RealType age_0,
               const SizeType n_hypnozoites)
    : current_(status),
      parasite_immunity_(parasite_immunity),
      boosts_parasite_immunity_(0.0),
      clinical_immunity_(clinical_immunity),
      boosts_clinical_immunity_(0.0),
      maternal_parasite_immunity_(maternal_parasite_immunity),
      maternal_clinical_immunity_(maternal_clinical_immunity),
      time_refractory_period_over_(std::numeric_limits<RealType>::lowest()),
      zeta_(zeta),
      omega_(1.0 - rho * std::exp(-age / age_0)),
      num_hypnozoites_(n_hypnozoites){};

void PVivax::queueInfection(const RealType t, const RealType lambda,
                            const RealType total_prob, const RealType delay) {
  auto num_new_hypnozoites = 0;
  auto bitten = genunf_std(generator) < lambda / total_prob;
  if (bitten) {
    // Track the number of hypnozoites that are made for this infection. Will be
    // added to the PVivax class when the infection is triggered.
    num_new_hypnozoites += 1;
  }
  infection_queue_.emplace(t + delay, num_new_hypnozoites);
};

bool PVivax::updateInfection(const RealType t, const RealType u_par) {
  if (infection_queue_.empty()) {
    return false;
  }

  auto activate_infection = t >= infection_queue_.top().getInfectionTime();
  if (!activate_infection) {
    return false;
  }

  while (!infection_queue_.empty() &&
         t >= infection_queue_.top().getInfectionTime()) {
    // For each infection that activates, we want to update the immunity and add
    // the appropriate number of hypnozoites.
    num_hypnozoites_ += infection_queue_.top().getNumHypnozoites();
    trackImmunityBoosts(t, u_par);
    infection_queue_.pop();
  }
  return true;
}

void PVivax::trackImmunityBoosts(const RealType t,
                                 const RealType refractory_period) noexcept {
  auto boosting_can_occur = t >= time_refractory_period_over_;
  if (boosting_can_occur) {
    ++boosts_parasite_immunity_;
    ++boosts_clinical_immunity_;
    time_refractory_period_over_ = t + refractory_period;
  }
}

void PVivax::updateImmunity(const RealType dt,
                            const RealType exp_rate_dt_parasite_immunity,
                            const RealType exp_rate_dt_clinical_immunity,
                            const RealType exp_rate_dt_maternal_immunity,
                            const RealType age,
                            const RealType end_maternal_immunity) noexcept {
  // Apply the decay rate onto immunity.
  parasite_immunity_ *= exp_rate_dt_parasite_immunity;
  parasite_immunity_ += boosts_parasite_immunity_;
  clinical_immunity_ *= exp_rate_dt_clinical_immunity;
  clinical_immunity_ += boosts_clinical_immunity_;
  maternal_parasite_immunity_ *= exp_rate_dt_maternal_immunity;

  auto zero_maternal_immunity = age >= end_maternal_immunity;
  if (zero_maternal_immunity) {
    maternal_parasite_immunity_ = 0.0;
    maternal_clinical_immunity_ = 0.0;
  }

  boosts_parasite_immunity_ = 0.0;
  boosts_clinical_immunity_ = 0.0;
}

void PVivax::clearInfectionQueue() noexcept {
  // It is faster to clear elements than replace with an empty queue.
  while (infection_queue_.size() != 0) {
    infection_queue_.pop();
  }
}
}  // namespace white
}  // namespace vivax
}  // namespace plasx