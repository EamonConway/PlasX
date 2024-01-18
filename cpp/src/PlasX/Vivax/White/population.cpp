#include "PlasX/Vivax/White/population.hpp"
namespace plasx {
namespace vivax {
namespace white {
Population::Population()
    : population_total_omega_zeta_(0.0),
      data_(),
      maternal_immunity_levels_(),
      default_maternal_immunity_levels_(
          std::vector<MaternalImmunity>(1, {0.0, 0.0})){};
}  // namespace white
}  // namespace vivax
}  // namespace plasx
