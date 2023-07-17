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
enum class Status { S, I_LM, I_PCR, I_D, T, P };

std::ostream& operator<<(std::ostream& os, const Status& data);
}  // namespace white
}  // namespace vivax
}  // namespace plasx
#endif