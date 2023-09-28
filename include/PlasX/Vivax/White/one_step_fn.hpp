/**
 * @file one_step.hpp
 * @author Eamon Conway (conway.e@wehi.edu.au)
 * @brief Source file for running a single time step of the equilibrium model.
 * @version 0.1
 * @date 2023-06-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef PLASX_VIVAX_WHITE_ONE_STEP_HPP
#define PLASX_VIVAX_WHITE_ONE_STEP_HPP
/**
 * @file one_step.hpp
 * @author Eamon Conway (conway.e@wehi.edu.au)
 * @brief
 * @version 0.1
 * @date 2023-06-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <type_traits>
#include "PlasX/Vivax/White/parameters.hpp"
#include "PlasX/Vivax/White/population.hpp"
#include "PlasX/Vivax/White/pvivax.hpp"
#include "PlasX/individual.hpp"
namespace plasx {
namespace vivax {
namespace white {
/**
 * @brief Runs a single step in time for the White model.
 * @details one_step uses a cache to store the current omega value using the
 * loop in the previous calculation. This can be problematic if multiple
 * simulations are run within the one source file. We may start the simulation
 * with an invalid cache. Currently the validity of the cache is checked using
 * the time, however if you were to change the population or mosquitoes and pass
 * in the same time, the cache would not be recognised as invalid.
 * It is plausible that in the future, we identify if the cache is valid using a
 * hash.
 * @param t The current time of the simulation (days).
 * @param dt The size of the time step (days).
 * @param population Storage class for all individuals and population level
 * parameters.
 * @param params Parameter class.
 * @param eir Current Entomological Inocculation Rate (assumed constant for time
 * step).
 * @return RealType The current time after the time step has taken place.
 */
struct one_step_fn {
  std::pair<std::unordered_map<Status, int>, RealType> operator()(
      const RealType t, const RealType dt, const RealType population_eir,
      Population& population, const Parameters& params) const;
};

inline constexpr one_step_fn one_step{};

}  // namespace white
}  // namespace vivax
}  // namespace plasx
#endif
