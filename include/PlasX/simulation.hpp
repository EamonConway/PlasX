#ifndef PLASX_SIMULATION_HPP
#define PLASX_SIMULATION_HPP
#include <concepts>

#include "PlasX/types.hpp"
namespace plasx {
// The simulation function
template <class OneStepFunction, class... OneStepArgs>
RealType simulation(const RealType t0, const RealType t1, const RealType dt,
                    OneStepFunction one_step, OneStepArgs&&... function_args) {
  // Call the appropriate overload using the OneStepFunctionSelector
  static_assert(std::invocable<OneStepFunction, const RealType, const RealType,
                               OneStepArgs...>);
  auto t = t0;
  while (t < t1) {
    t = one_step(t, dt, std::forward<OneStepArgs>(function_args)...);
  }
  return t;
}
}  // namespace plasx
#endif