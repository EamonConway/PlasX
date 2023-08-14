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
 * @param t
 * @param dt
 * @param population
 * @param params
 * @param eir
 * @return RealType
 */
struct one_step_fn {
  std::unordered_map<Status, int> operator()(
      RealType& t, RealType dt, std::vector<Individual<PVivax>>& population,
      const Parameters& params, RealType eir) const;

  template <class EirFunction>
    requires std::is_invocable_r_v<RealType, EirFunction, const RealType>
  std::unordered_map<Status, int> operator()(
      RealType& t, RealType dt, std::vector<Individual<PVivax>>& population,
      const Parameters& params, EirFunction eir_func) const {
    RealType eir = eir_func(t);
    return operator()(t, dt, population, params, eir);
  };
};

inline constexpr one_step_fn one_step{};
}  // namespace white
}  // namespace vivax
}  // namespace plasx
#endif