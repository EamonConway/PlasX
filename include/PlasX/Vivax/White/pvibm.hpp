#ifndef PLASX_VIVAX_WHITE_PVIBM_HPP
#define PLASX_VIVAX_WHITE_PVIBM_HPP
#include "PlasX/Mosquito/multispecies_mosquito_model.hpp"
#include "PlasX/Vivax/White/mosquito_update.hpp"
#include "PlasX/Vivax/White/one_step_fn.hpp"
#include "PlasX/model_simulation_fn.hpp"
namespace plasx {
namespace pvibm {
inline constexpr model_simulation_fn equilibrium(vivax::white::one_step_fn{},
                                                 [](RealType, RealType,
                                                    RealType, RealType eir) {
                                                   return std::make_pair(0,
                                                                         eir);
                                                 });

inline constexpr model_simulation_fn general_mosquito_ode_ibm_model(
    vivax::white::one_step_fn{}, mosquito::MultiSpeciesMosquitoOdeFn{});

inline constexpr model_simulation_fn simple_mosquito_ode_ibm_model(
    vivax::white::one_step_fn{}, [](auto&&... params) {
      return vivax::white::simple_mosquito_update(std::forward(params...));
    });
}  // namespace pvibm
}  // namespace plasx
#endif
