#include "PlasX/Vivax/White/population.hpp"
namespace plasx {
namespace vivax {
namespace white {

Population::Population()
    : Population(std::vector<MaternalImmunity>(1, {0.0, 0.0})){};

Population::Population(std::vector<MaternalImmunity>&& default_immunity_level)
    : data_(),
      maternal_immunity_levels_(),
      default_maternal_immunity_levels_(std::move(default_immunity_level)),
      population_total_omega_zeta_(0.0){};

Population::Population(std::vector<PersonType>&& population)
    : Population(std::move(population),
                 std::vector<MaternalImmunity>(1, {0.0, 0.0})){};

Population::Population(std::vector<PersonType>&& population,
                       std::vector<MaternalImmunity>&& default_immunity_level)
    : data_(std::move(population)),
      maternal_immunity_levels_(),
      default_maternal_immunity_levels_(std::move(default_immunity_level)),
      population_total_omega_zeta_(0.0) {
  // Calculate the population level parameters for use in the simulation.
  // These parameters must be kept up to date by the programmer and are not
  // automatic with the api.
  for (const auto& person : data_) {
    population_total_omega_zeta_ +=
        person.status_.getOmega() * person.status_.getZeta();
    maternal_immunity_levels_.emplace_back(
        person.status_.getParasiteImmunity(),
        person.status_.getClinicalImmunity());
  }
};
}  // namespace white
}  // namespace vivax
}  // namespace plasx
