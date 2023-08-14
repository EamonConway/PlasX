#ifndef PLASX_SIMULATION_HPP
#define PLASX_SIMULATION_HPP
#include <concepts>

#include "PlasX/types.hpp"
namespace plasx {
template <class Fn, class... Args>
  requires std::invocable<Fn, RealType&, const RealType, Args...>
auto simulation(const RealType t0, const RealType t1, const RealType dt,
                Fn&& one_step, Args&&... function_args) {
  auto t = t0;
  using OneStepResultType =
      std::invoke_result_t<Fn, RealType&, const RealType, Args...>;
  using OutputType =
      std::pair<std::vector<RealType>, std::vector<OneStepResultType>>;

  // Create the empty output
  auto output_value = OutputType{};
  while (t < t1) {
    // Do we want one_step to update t?
    auto output = one_step(t, dt, std::forward<Args>(function_args)...);
    // We can add a check to determine if we want to log this timestep or not.
    output_value.first.push_back(t);
    output_value.second.push_back(output);
  }
  return output_value;
}
}  // namespace plasx
#endif