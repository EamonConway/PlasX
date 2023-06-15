#ifndef PLASX_SIMULATION_HPP
#define PLASX_SIMULATION_HPP
#include <algorithm>

#include "PlasX/types.hpp"

namespace plasx {

/**
 * @brief
 *
 * @tparam OneStepFunction
 * @tparam OneStepArgs
 * @param t0
 * @param t1
 * @param dt
 * @param one_step
 * @param function_args
 * @return RealType
 */
template <class OneStepFunction, class... OneStepArgs>
RealType simulation(const double t0, const double t1, const double dt,
                    OneStepFunction one_step, OneStepArgs&&... function_args) {
  auto t = t0;

  while (t < t1) {
    t = one_step(t, dt,
                 std::forward<decltype(function_args)>(function_args)...);
  }
  return t;
}
}  // namespace plasx
#endif