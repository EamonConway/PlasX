#include "PlasX/random.hpp"

namespace plasx {
// Allows for a different random seeding method on Windows. _WIN32 should be
// defined even on 64 bit.
#ifdef _WIN32
#include <chrono>
std::mt19937 generator(
    std::chrono::system_clock::now().time_since_epoch().count());
#else
std::mt19937 generator(std::random_device{}());
#endif

std::uniform_real_distribution<double> genunf_std(0.0, 1.0);
}  // namespace plasx