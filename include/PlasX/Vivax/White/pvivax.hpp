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
  PVivax(RealType age, Status status, RealType parasite_immunity,
         RealType clinical_immunity, RealType maternal_parasite_immunity,
         RealType maternal_clinical_immunity, RealType zeta, RealType rho,
         RealType age_0, SizeType n_hypnozoites);

  /**
   * @brief Construct a new PVivax object - should be used for the birth of an
   * individual.
   *
   * @param status
   * @param maternal_parasite_immunity
   * @param maternal_clinical_immunity
   * @param zeta
   */
  PVivax(RealType age, Status status, RealType maternal_parasite_immunity,
         RealType maternal_clinical_immunity, RealType zeta, RealType rho,
         RealType age_0);

  /**
   * @brief What happens when an infection is triggered.
   *
   */
  void queueInfection(RealType t, RealType lambda, RealType total_prob,
                      RealType delay);

  /**
   * @brief Check if an infection event has to occur.
   *
   * @param t
   * @param u_par
   * @return true
   * @return false
   */
  bool updateInfection(RealType t, RealType u_par);

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

  void updateImmunity(RealType dt, RealType exp_rate_dt_parasite_immunity,
                      RealType exp_rate_dt_clinical_immunity,
                      RealType exp_rate_dt_maternal_immunity, RealType age,
                      RealType end_maternal_immunity) noexcept;

  /**
   * @brief Get the heterogeneity in biting for an individual.
   *
   * @return RealType
   */
  const RealType& getZeta() const noexcept { return zeta_; };

  const RealType& getOmega() const noexcept { return omega_; };
  void setOmega(RealType omega) noexcept { omega_ = omega; };

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

  struct PrivateData {
    RealType time_refractory_period_over;
    InfectionScheduler infection_queue;
  };

  PrivateData copyPrivateData() {
    return {time_refractory_period_over_, infection_queue_};
  };

  struct CompressedData {
    float zeta, parasite_immunity, clinical_immunity,
        maternal_parasite_immunity, maternal_clinical_immunity;
    u_int32_t nhyp;
    Status state;
  };

  [[gnu::used]] CompressedData compressedOutput() const {
    return {static_cast<float>(zeta_),
            static_cast<float>(parasite_immunity_),
            static_cast<float>(clinical_immunity_),
            static_cast<float>(maternal_parasite_immunity_),
            static_cast<float>(maternal_clinical_immunity_),
            static_cast<u_int32_t>(num_hypnozoites_),
            current_};
  }

 private:
  /**
   * @brief
   *
   * @param t
   * @param refractory_period
   */
  void trackImmunityBoosts(RealType t, RealType refractory_period) noexcept;

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
