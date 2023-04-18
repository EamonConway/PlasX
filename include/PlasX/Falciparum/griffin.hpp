#ifndef PLASX_FALCIPARUM_GRIFFIN_HPP
#define PLASX_FALCIPARUM_GRIFFIN_HPP
/**
 * @file griffin.hpp
 * @author Eamon Conway (conway.e@wehi.edu.au)
 * @brief
 * @version 0.1
 * @date 2023-03-31
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <queue>

#include "PlasX/Falciparum/Griffin/parameters.h"
#include "PlasX/individual.hpp"

namespace plasx {
namespace falciparum {
namespace griffin {

/**
 * @brief Enum for the different states an individual can occupy.
 *
 */
enum class Status { S, A, U, D, T, P };

/**
 * @brief Class defining all individual level variables in the model of Griffin
 * et al.
 *
 */
class PFalc {
 public:
  /**
   * @brief Construct a new PFalc object
   *
   * @param status Set the initial state of the individual.
   */
  PFalc(const Status& status, double ICA, double ICM, double IA);

  /**
   * @brief Clear the queue of infections that will occur. This is only called
   * when an individual will go into the treated compartment.
   *
   */
  void clearInfectionQueue() noexcept;

  /**
   * @brief Schedule an infection event for time t.
   *
   * @param t Time of infection event.
   */
  void scheduleInfection(const double t);

  /**
   * @brief Check if an infection event has to occur.
   *
   * @param t
   * @return true
   * @return false
   */
  bool updateInfection(const double t);

  /**
   * @brief Get the immunity level from stored state.
   *
   * @return double
   */
  double getIC() noexcept;

  /**
   * @brief Get the immunity level from stored state.
   *
   * @return double
   */
  double getIA() noexcept;

  /**
   * @brief Update the individual.
   * @details This function updates the state of an individual by a single
   * timestep.
   */
  std::function<bool(const double, const double)> update_;

  /**
   * @brief Current state of the individual.
   *
   */
  Status current_;

 private:
  double I_CA_;
  double I_CM_;
  double I_A_;

  // I am assuming that you only want the force of infection to be lagged, hence
  // we only have to store the time of the next infection.
  double cached_infection_;
  std::priority_queue<double, std::vector<double>, std::less<double>>
      infection_queue_;
};

/**
 * @brief Runs a single step in time for the Griffin model.
 *
 * @param population
 * @param params
 * @return RealType containing the current time.
 */
RealType one_step(std::vector<Individual<PFalc>>& population,
                  const Parameters& params);

}  // namespace griffin
}  // namespace falciparum
}  // namespace plasx

#endif