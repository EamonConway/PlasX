#ifndef PLASX_VIVAX_WHITE_MOSQUITO_HPP
#define PLASX_VIVAX_WHITE_MOSQUITO_HPP
#include <array>
#include <queue>
#include <type_traits>

#include "PlasX/Vivax/White/mosquito_parameters.hpp"
#include "PlasX/types.hpp"
namespace plasx {
namespace vivax {
namespace white {
class MosquitoModel {
 public:
  std::array<RealType, 6> operator()(const RealType t,
                                     const std::array<RealType, 6>& state,
                                     const RealType lambda,
                                     const RealType kappa,
                                     const MosquitoParameters& params) const;

  template <typename SeasonalFn>
    requires std::is_invocable_r_v<RealType, SeasonalFn, const RealType>
  std::array<RealType, 6> operator()(const RealType t,
                                     const std::array<RealType, 6>& state,
                                     const RealType lambda,
                                     const SeasonalFn& kappa,
                                     const MosquitoParameters& params) const {
    auto k = kappa(t);
    return this->operator()(t, state, lambda, k, params);
  };
};
}  // namespace white
}  // namespace vivax
}  // namespace plasx
#endif