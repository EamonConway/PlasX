#ifndef PLASX_VIVAX_WHITE_MOSQUITO_HPP
#define PLASX_VIVAX_WHITE_MOSQUITO_HPP
#include <array>
#include <queue>

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
};
}  // namespace white
}  // namespace vivax
}  // namespace plasx
#endif