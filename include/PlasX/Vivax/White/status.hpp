/**
 * @file status.hpp
 * @author Eamon Conway (conway.e@wehi.edu.au)
 * @brief
 * @version 0.1
 * @date 2023-06-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef PLASX_VIVAX_WHITE_STATUS_HPP
#define PLASX_VIVAX_WHITE_STATUS_HPP
#include <iostream>
namespace plasx {
namespace vivax {
namespace white {
/**
 * @brief Enum for the different states an individual can occupy.
 *
 */
enum class Status : uint8_t {
  S = 0,
  I_LM = 1,
  I_PCR = 2,
  I_D = 3,
  T = 4,
  P = 5,
};

std::ostream& operator<<(std::ostream& os, const Status& data);
}  // namespace white
}  // namespace vivax
}  // namespace plasx
#endif
