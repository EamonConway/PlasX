#ifndef PLASX_VIVAX_WHITE_PVIBM_HPP
#define PLASX_VIVAX_WHITE_PVIBM_HPP
#include "PlasX/Vivax/White/one_step.hpp"
#include "PlasX/model_simulation.hpp"
namespace plasx {
namespace pvibm {
inline constexpr model_simulation_fn equilibrium(
    vivax::white::one_step_fn{}, [](RealType t, RealType dt, RealType unused, RealType eir) {
      return std::make_pair(0, eir);
    });
}
}  // namespace plasx
#endif
