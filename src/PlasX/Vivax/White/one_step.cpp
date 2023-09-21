/** @file one_step.cpp @author Eamon Conway (conway.e@wehi.edu.au)
 * @brief
 * @version 0.1 @date 2023-06-23 @copyright Copyright (c) 2023
 *
 */
#include "PlasX/Vivax/White/one_step.hpp"

#include <cmath>
#include <utility>

#include "PlasX/Vivax/White/individual_update.hpp"
#include "PlasX/Vivax/White/population.hpp"
#include "PlasX/random.hpp"
#include "PlasX/udl.hpp"

namespace plasx {
namespace vivax {
namespace white {
std::pair<std::unordered_map<Status, int>, RealType> one_step_fn::operator()(
    const RealType t, const RealType dt, const RealType population_eir,
    Population& population, const Parameters& params) const {
  // Parameters for rho and age.
  const auto &rho = params.rho, &age_0 = params.age_0;

  // Get biting parameters to calculate Lambda
  const auto eir = population_eir / population.size();
  const auto total_omega_zeta = population.total_omega_zeta(),
             eir_omega_zeta = eir / total_omega_zeta;

  // Variables to be calculated during loop over individuals
  auto total_foi_human_to_mosquitoes = 0.0;
  auto cacheable_omega_zeta = 0.0;
  std::vector<Population::MaternalImmunity> cacheable_maternal_immunity;
  cacheable_maternal_immunity.reserve(population.size() / 4);

  // Output data storage - TimeStepLogger?
  std::unordered_map<Status, int> data_logger{
      {Status::S, 0},   {Status::I_PCR, 0}, {Status::I_LM, 0},
      {Status::I_D, 0}, {Status::T, 0},     {Status::P, 0}};

  // Birth an equal amount of individuals to those that died.
  // We need to randomly determine who the new individual's birthing person is.
  const auto& maternal_immunity = population.get_maternal_immunity();
  auto gen_birthing_person =
      std::uniform_int_distribution<SizeType>(0, maternal_immunity.size() - 1);
  auto gen_zeta = std::lognormal_distribution<RealType>(
      params.biting_rate_log_mean, params.biting_rate_log_sd);

  // Update the state of each individual one by one - cache any values that will
  // be of use in the next time step
  std::for_each(
      population.begin(), population.end(),
      [&](Population::PersonType& person) -> void {
        auto [individual_dies, individual_foi_mosquitoes] =
            IndividualOneStep(t, dt, person, params, eir_omega_zeta);
        total_foi_human_to_mosquitoes += individual_foi_mosquitoes;

        // If the individual dies, we want to replace them with a
        // newborn - they will be born with immunity taken from the
        // distribution of individuals at the start of this timestep.
        if (individual_dies) {
          // Birth a new individual.
          const auto birthing_person_ref = gen_birthing_person(generator);
          const auto& [parasite_immunity, clinical_immunity] =
              maternal_immunity[birthing_person_ref];

          // Create the new individual with some proportion of their
          // birthing persons immunity
          const auto zeta = gen_zeta(generator);
          person = Population::PersonType(
              0.0, Status::S,
              params.proportion_maternal_immunity * parasite_immunity,
              params.proportion_maternal_immunity * clinical_immunity, zeta,
              rho, age_0);
        }

        // Update cached information
        const auto individual_is_birthing_capable = person.isBirthingCapable(
            params.min_birth_age, params.max_birth_age);
        if (individual_is_birthing_capable) {
          // Track the immunity levels of all birthing capable
          // individuals to sample from at the time of birth.
          cacheable_maternal_immunity.emplace_back(
              person.status_.getParasiteImmunity(),
              person.status_.getClinicalImmunity());
        }
        cacheable_omega_zeta +=
            person.status_.getOmega() * person.status_.getZeta();

        // Log the current status of the individual for
        // outputting.
        ++data_logger[person.status_.current_];
      });

  // Cache variables for any functions that will be required.
  population.set_maternal_immunity(std::move(cacheable_maternal_immunity));
  population.total_omega_zeta() = cacheable_omega_zeta;

  return std::make_pair(data_logger, total_foi_human_to_mosquitoes);
};
}  // namespace white
}  // namespace vivax
}  // namespace plasx
