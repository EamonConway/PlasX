#ifndef PLASX_VIVAX_WHITE_MOSQUITO_UPDATE_HPP
#define PLASX_VIVAX_WHITE_MOSQUITO_UPDATE_HPP
#include <unordered_map>

#include "PlasX/Mosquito/simple_mosquito_parameters.hpp"
namespace plasx {
namespace vivax {
namespace white {
std::pair<std::unordered_map<plasx::MosquitoSpecies, std::array<RealType, 3>>,
          RealType>
simple_mosquito_update(
    double dt, double t0, double lambda,
    const std::unordered_map<plasx::MosquitoSpecies, std::array<RealType, 3>>&
        initial_state,
    const std::unordered_map<plasx::MosquitoSpecies,
                             mosquito::SimpleMosquitoParameters>&
        mosquito_parameters);
}  // namespace white
}  // namespace vivax
}  // namespace plasx
#endif
