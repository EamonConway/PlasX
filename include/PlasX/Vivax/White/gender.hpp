/**
 * @file gender.hpp
 * @author Eamon Conway (conway.e@wehi.edu.au)
 * @brief
 * @version 0.1
 * @date 2023-07-24
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
 * @brief
 *
 */
enum class Gender { AFAB, AMAB };
std::ostream& operator<<(std::ostream& os, const Gender& data);
}  // namespace white
}  // namespace vivax
}  // namespace plasx
#endif