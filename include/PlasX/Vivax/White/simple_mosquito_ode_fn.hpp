#ifndef PLASX_VIVAX_WHITE_MOSQUITO_HPP
#define PLASX_VIVAX_WHITE_MOSQUITO_HPP
#include <array>

#include "PlasX/Vivax/White/simple_mosquito_parameters.hpp"
#include "PlasX/types.hpp"
namespace plasx {
namespace vivax {
namespace white {
struct simple_mosquito_ode_fn {
  std::array<RealType, 3> operator()(
      const RealType t, const std::array<RealType, 3>& state,
      const RealType lambda, const SimpleMosquitoParameters& params) const;
};
}  // namespace white
}  // namespace vivax
}  // namespace plasx
#endif
