#ifndef PLASX_INDIVIDUAL_HPP
#define PLASX_INDIVIDUAL_HPP
/**
 * @file individual.hpp
 * @author Eamon Conway (conway.e@wehi.edu.au)
 * @brief
 * @version 0.1
 * @date 2023-03-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "PlasX/types.hpp"
namespace plasx {
/**
 * @details Individuals depend upon their underlying disease status.
 * We use their disease status as a template parameter so that we can
 * efficiently change the type of individual that we run the simulation for.
 * @tparam DiseaseStatus is a class that stores all individual specific
 * information for the disease.
 */
template <typename DiseaseStatus>
class Individual {
 public:
  /**
   * @brief Construct a new Individual object
   *
   * @param age
   * @param dargs Parameter pack that is forwarded to the constructor of the
   * underlying status_.
   */
  template <typename... DiseaseArgs>
  Individual(double age, DiseaseArgs&&... dargs)
      : age_(age), status_(age, std::forward<DiseaseArgs>(dargs)...){};

  constexpr const bool isBirthingCapable(double min_age, double max_age) const {
    return age_ >= min_age && age_ <= max_age;
  }

  constexpr bool isBirthingCapable(double min_age, double max_age) {
    return age_ >= min_age && age_ <= max_age;
  }

  RealType age_;
  DiseaseStatus status_;
};
}  // namespace plasx
#endif
