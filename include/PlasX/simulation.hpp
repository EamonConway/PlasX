#ifndef PLASX_SIMULATION_HPP
#define PLASX_SIMULATION_HPP
#include <algorithm>
#include <concepts>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

#include "PlasX/types.hpp"
namespace plasx {
// Function object to select the correct OneStepFunction overload
template <class OneStepFunction, class... OneStepArgs>
struct OneStepFunctionSelector {
  static auto call(OneStepFunction&& func, OneStepArgs&&... args) {
    // Check if invoking func with the provided arguments is valid
    static_assert(
        std::invocable<OneStepFunction, OneStepArgs...>,
        "Type OneStepFunction is not invocable with the arguments provided.");

    // Call the appropriate overload using the OneStepFunctionSelector
    return std::invoke(std::forward<OneStepFunction>(func),
                       std::forward<OneStepArgs>(args)...);
  }
};

// The simulation function
template <class OneStepFunction, class... OneStepArgs>
RealType simulation(const RealType t0, const RealType t1, const RealType dt,
                    OneStepFunction one_step, OneStepArgs&&... function_args) {
  // Call the appropriate overload using the OneStepFunctionSelector
  auto t = t0;
  while (t < t1) {
    t = OneStepFunctionSelector<
        OneStepFunction, const RealType&, const RealType&,
        OneStepArgs...>::call(std::forward<OneStepFunction>(one_step),t,dt,
                              std::forward<OneStepArgs>(function_args)...);
  }
  return t;
}
}  // namespace plasx
#endif