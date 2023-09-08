#ifndef PLASX_VIVAX_WHITE_INDIVIDUAL_ONE_STEP_HPP
#define PLASX_VIVAX_WHITE_INDIVIDUAL_ONE_STEP_HPP
#include "PlasX/Vivax/White/parameters.hpp"
#include "PlasX/Vivax/White/pvivax.hpp"
#include "PlasX/individual.hpp"
namespace plasx {
namespace vivax {
namespace white {

struct IndividualOneStepReturnType {
  bool isDead;
  double c;  // Contribution to force of infection on mosquitoes.
};

IndividualOneStepReturnType IndividualOneStep(const RealType t,
                                              const RealType dt,
                                              Individual<PVivax>& person,
                                              const Parameters& params,
                                              const RealType scaled_eir);
}  // namespace white
}  // namespace vivax
}  // namespace plasx
#endif
