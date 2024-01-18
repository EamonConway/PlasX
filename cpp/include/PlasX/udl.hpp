#ifndef PLASX_UDL_HPP
#define PLASX_UDL_HPP

namespace plasx {
constexpr long double operator"" _yrs(long double years) {
  return years * 365.0;
}

constexpr long double operator"" _days(long double x) { return x; };
}  // namespace plasx
#endif