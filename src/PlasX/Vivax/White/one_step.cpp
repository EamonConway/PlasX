/** @file one_step.cpp @author Eamon Conway (conway.e@wehi.edu.au)
 * @brief
 * @version 0.1 @date 2023-06-23 @copyright Copyright (c) 2023
 *
 */
#include "PlasX/Vivax/White/one_step.hpp"

#include <cmath>
#include <utility>

#include "PlasX/Vivax/White/individual_update.hpp"
#include "PlasX/random.hpp"
#include "PlasX/udl.hpp"

namespace {
using RealType = plasx::RealType;
using Person = plasx::Individual<plasx::vivax::white::PVivax>;

struct MaternalImmunity {
  double parasite_immunity;
  double clinical_immunity;
  MaternalImmunity(RealType parasite, RealType clinical)
      : parasite_immunity(parasite), clinical_immunity(clinical){};
};

// We are going to have to determine how caching will work in regards to the
// python API that is being developed.
class CachedVariables {
 public:
  CachedVariables(std::vector<MaternalImmunity>&& maternal_immunity,
                  RealType omega_zeta, RealType time)
      : maternal_immunity_(std::move(maternal_immunity)),
        omega_zeta_(omega_zeta),
        valid_time_(time) {
    // If the maternal_immunity_store is empty, we add zero to it before caching
    // the next timestep. This is the edge case of no birthing persons being
    // present, which whilst unphysical, may sometimes occur within the code -
    // especially during the burn in phase if we have to initialise a large
    // amount of individuals at 0 years of age.
    if (maternal_immunity_.empty()) {
      maternal_immunity_.emplace_back(0.0, 0.0);
    }
  };
  std::vector<MaternalImmunity> maternal_immunity_;
  RealType omega_zeta_;

  bool isCacheInvalid(RealType t) { return t != valid_time_; }

 private:
  RealType valid_time_;
};

std::optional<CachedVariables> kcached_data;

void calculateRequiredCache(const std::vector<Person>& population, RealType rho,
                            RealType age_0, RealType t) {
  // We need to get the force of infection from mosquito to humans, then we
  // need to calculate the age normalisation constant.
  auto cacheable_omega_zeta = 0.0;
  std::vector<MaternalImmunity> maternal_immunity;
  for (const auto& person : population) {
    cacheable_omega_zeta +=
        person.status_.getOmega() * person.status_.getZeta();
    maternal_immunity.emplace_back(person.status_.getParasiteImmunity(),
                                   person.status_.getClinicalImmunity());
  }
  kcached_data.emplace(std::move(maternal_immunity), cacheable_omega_zeta, t);
};
}  // namespace

namespace plasx {
namespace vivax {
namespace white {
std::unordered_map<Status, int> one_step_fn::operator()(
    RealType& t, RealType dt, RealType population_eir,
    std::vector<Individual<PVivax>>& population,
    const Parameters& params) const {
  // We need the value for omega within this timestep, however, it requires
  // knowing information over the whole population. As such we calculate these
  // details during the previous timestep. It is not guaranteed that there has
  // been a previous timestep, so we have used an optional to determine when to
  // update the cache.
  const auto cache_is_empty = !kcached_data.has_value();
  const auto cache_needs_calculation =
      cache_is_empty || kcached_data.value().isCacheInvalid(t);
  if (cache_needs_calculation) {
    // Recalculate the cache when it is out of date - currently determined by
    // time only.
    calculateRequiredCache(population, params.rho, params.age_0, t);
  }

  // Get biting parameters to calculate Lambda
  const auto eir = population_eir / population.size();
  const auto rho = params.rho, age_0 = params.age_0;
  const auto total_omega_zeta = kcached_data.value().omega_zeta_,
             eir_omega_zeta = eir / total_omega_zeta;

  // Variables to be calculated during loop over individuals
  auto total_foi_human_to_mosquitoes = 0.0;
  auto cacheable_omega_zeta = 0.0;
  std::vector<MaternalImmunity> cacheable_maternal_immunity;
  cacheable_maternal_immunity.reserve(population.size() / 4);

  // Output data storage - TimeStepLogger?
  std::unordered_map<Status, int> data_logger{
      {Status::S, 0},   {Status::I_PCR, 0}, {Status::I_LM, 0},
      {Status::I_D, 0}, {Status::T, 0},     {Status::P, 0}};

  // Birthing updaters.
  // Birth an equal amount of individuals to those that died.
  // We need to randomly determine who the new individual's birthing person is.
  const auto& maternal_immunity = kcached_data.value().maternal_immunity_;
  auto gen_birthing_person =
      std::uniform_int_distribution<SizeType>(0, maternal_immunity.size() - 1);
  auto gen_zeta = std::lognormal_distribution<RealType>(
      params.biting_rate_log_mean, params.biting_rate_log_sd);

  // Update the state of each individual one by one - cache any values that will
  // be of use in the next time step
  std::for_each(
      population.begin(), population.end(), [&](Person& person) -> void {
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
          person =
              Person(0.0, Status::S,
                     params.proportion_maternal_immunity * parasite_immunity,
                     params.proportion_maternal_immunity * clinical_immunity,
                     zeta, rho, age_0);
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
        // Omega caching.
        cacheable_omega_zeta +=
            person.status_.getOmega() * person.status_.getZeta();
        // Log the current status of the individual for
        // outputting.
        ++data_logger[person.status_.current_];
      });

  // Update timestep
  t += dt;
  // Store cached values and data required to confirm validity of the cache
  kcached_data.emplace(std::move(cacheable_maternal_immunity),
                       cacheable_omega_zeta,
                       t);  // Cache latest timestep for validity.
  return data_logger;
};
}  // namespace white
}  // namespace vivax
}  // namespace plasx
