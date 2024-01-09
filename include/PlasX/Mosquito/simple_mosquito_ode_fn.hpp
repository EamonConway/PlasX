#ifndef PLASX_VIVAX_WHITE_MOSQUITO_ODE_FN_HPP
#define PLASX_VIVAX_WHITE_MOSQUITO_ODE_FN_HPP
#include <array>

#include "PlasX/Mosquito/simple_mosquito_parameters.hpp"
#include "PlasX/types.hpp"
namespace plasx {
namespace mosquito {
struct simple_mosquito_ode_fn {
  std::array<RealType, 3> operator()(
      RealType t, const std::array<RealType, 3>& state, RealType lambda,
      const SimpleMosquitoParameters& params) const;
};

inline constexpr simple_mosquito_ode_fn simple_mosquito_ode{};
}  // namespace mosquito
}  // namespace plasx
#endif
