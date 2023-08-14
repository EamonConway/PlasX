/**
 * @file pvivax.hpp
 * @author Eamon Conway (conway.e@wehi.edu.au)
 * @brief
 * @version 0.1
 * @date 2023-06-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef PLASX_VIVAX_WHITE_PVIVAX_HPP
#define PLASX_VIVAX_WHITE_PVIVAX_HPP
#include <iostream>

#include "PlasX/Vivax/White/infection_scheduler.hpp"
#include "PlasX/Vivax/White/parameters.hpp"
#include "PlasX/Vivax/White/status.hpp"
#include "PlasX/individual.hpp"

namespace plasx {
namespace vivax {
namespace white {
/**
 * @brief Class containing all individual specific information for the White
 * model.
 *
 */
class PVivax {
 public:
  /**
   * @brief Construct a new PVivax object
   *
   * @param status
   * @param parasite_immunity
   * @param clinical_immunity
   * @param maternal_parasite_immunity
   * @param maternal_clinical_immunity
   * @param zeta
   */
  PVivax(const RealType age, const Status& status,
         const RealType parasite_immunity, const RealType clinical_immunity,
         const RealType maternal_parasite_immunity,
         const RealType maternal_clinical_immunity, const RealType zeta,
         const RealType rho, const RealType age_0,
         const SizeType n_hypnozoites);

  /**
   * @brief Construct a new PVivax object - should be used for the birth of an
   * individual.
   *
   * @param status
   * @param maternal_parasite_immunity
   * @param maternal_clinical_immunity
   * @param zeta
   */
  PVivax(const RealType age, const Status& status,
         const RealType maternal_parasite_immunity,
         const RealType maternal_clinical_immunity, const RealType zeta,
         const RealType rho, const RealType age_0);

  /**
   * @brief What happens when an infection is triggered.
   *
   */
  void queueInfection(const RealType t, const RealType lambda,
                      const RealType total_prob, const RealType delay);

  /**
   * @brief Check if an infection event has to occur.
   *
   * @param t
   * @param u_par
   * @return true
   * @return false
   */
  bool updateInfection(const RealType t, const RealType u_par);

  SizeType& getNumHypnozoites() noexcept { return num_hypnozoites_; }
  const SizeType& getNumHypnozoites() const noexcept {
    return num_hypnozoites_;
  }

  /**
   * @brief Clear the queue of infections that will occur. This is only called
   * when an individual will go into the treated compartment.
   *
   */
  void clearInfectionQueue() noexcept;

  /**
   * @brief
   *
   * @param t
   * @param refractory_period
   */
  void trackImmunityBoosts(const RealType t,
                           const RealType refractory_period) noexcept;

  void update(const RealType dt);

  void updateImmunity(const RealType dt,
                      const RealType exp_rate_dt_parasite_immunity,
                      const RealType exp_rate_dt_clinical_immunity,
                      const RealType exp_rate_dt_maternal_immunity,
                      const RealType age,
                      const RealType end_maternal_immunity) noexcept;

  /**
   * @brief Get the heterogeneity in biting for an individual.
   *
   * @return RealType
   */
  const RealType& getZeta() const noexcept { return zeta_; };

  const RealType& getOmega() const noexcept { return omega_; };
  void setOmega(const RealType& omega) noexcept { omega_ = omega; };

  // get functions call the old levels of immunity as the others have been
  // updated slightly in the switch
  const RealType getParasiteImmunity() const noexcept {
    return parasite_immunity_ + maternal_parasite_immunity_;
  };
  const RealType getClinicalImmunity() const noexcept {
    return clinical_immunity_ + maternal_clinical_immunity_;
  };

  /**
   * @brief Current state of the individual.
   *
   */
  Status current_;

 private:
  RealType parasite_immunity_, boosts_parasite_immunity_;
  RealType clinical_immunity_, boosts_clinical_immunity_;
  RealType maternal_parasite_immunity_;
  RealType maternal_clinical_immunity_;
  RealType time_refractory_period_over_;
  RealType zeta_;
  RealType omega_;
  SizeType num_hypnozoites_;
  InfectionScheduler infection_queue_;
  friend std::ostream& operator<<(std::ostream& os, const PVivax& data);
};
}  // namespace white
}  // namespace vivax
}  // namespace plasx
#endif