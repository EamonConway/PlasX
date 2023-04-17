#ifndef PLASX_RANDOM_HPP
#define PLASX_RANDOM_HPP
/**
 * @file random.hpp
 * @author Eamon Conway (conway.e@wehi.edu.au)
 * @brief
 * @version 0.1
 * @date 2023-03-31
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <random>

namespace plasx {
extern std::uniform_real_distribution<double> gen_unf;
extern std::default_random_engine generator;
}  // namespace plasx
#endif