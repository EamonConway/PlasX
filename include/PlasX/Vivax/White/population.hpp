#ifndef PLASX_VIVAX_WHITE_POPULATION_HPP
#define PLASX_VIVAX_WHITE_POPULATION_HPP
#include <vector>

#include "PlasX/Vivax/White/pvivax.hpp"
#include "PlasX/individual.hpp"
#include "PlasX/types.hpp"
namespace plasx {
namespace vivax {
namespace white {
class Population {
 public:
  using PersonType = Individual<PVivax>;
  using iterator = std::vector<PersonType>::iterator;
  using const_iterator = std::vector<PersonType>::const_iterator;
  using size_type = std::vector<PersonType>::size_type;

  struct MaternalImmunity {
    double parasite_immunity;
    double clinical_immunity;
    MaternalImmunity(RealType parasite, RealType clinical)
        : parasite_immunity(parasite), clinical_immunity(clinical){};
  };

  Population();

  [[nodiscard("iterator cannot be discarded")]] constexpr iterator begin() {
    return data_.begin();
  };
  [[nodiscard("const_iterator cannot be discarded")]] constexpr const_iterator
  begin() const {
    return data_.begin();
  };
  [[nodiscard("iterator cannot be discarded")]] constexpr iterator end() {
    return data_.end();
  };
  [[nodiscard("const_iterator cannot be discarded")]] constexpr const_iterator
  end() const {
    return data_.end();
  };

  constexpr RealType& total_omega_zeta() {
    return population_total_omega_zeta_;
  };
  constexpr const RealType& total_omega_zeta() const {
    return population_total_omega_zeta_;
  };

  constexpr size_type size() { return data_.size(); };
  constexpr size_type size() const { return data_.size(); };

  template <typename... IndividualArgs>
  void emplace_back(double min_age, double max_age, IndividualArgs&&... args) {
    const auto& person =
        data_.emplace_back(std::forward<IndividualArgs>(args)...);
    const auto& state = person.status_;
    const auto person_is_birthing_capable =
        person.isBirthingCapable(min_age, max_age);
    if (person_is_birthing_capable) {
      maternal_immunity_levels_.emplace_back(
          person.status_.getParasiteImmunity(),
          person.status_.getClinicalImmunity());
    }
    population_total_omega_zeta_ += state.getOmega() * state.getZeta();
  }

  void set_maternal_immunity(
      std::vector<MaternalImmunity>&& maternal_immunity) {
    maternal_immunity_levels_ = std::move(maternal_immunity);
  };

  // Do we need to provide some lifetime guarantee here for the best practice?
  constexpr const auto& get_maternal_immunity() {
    if (maternal_immunity_levels_.empty()) {
      return default_maternal_immunity_levels_;
    }
    return maternal_immunity_levels_;
  };

 private:
  RealType population_total_omega_zeta_;
  std::vector<Individual<PVivax>> data_;
  std::vector<MaternalImmunity> maternal_immunity_levels_;
  const std::vector<MaternalImmunity> default_maternal_immunity_levels_;
};
}  // namespace white
}  // namespace vivax
}  // namespace plasx
#endif
