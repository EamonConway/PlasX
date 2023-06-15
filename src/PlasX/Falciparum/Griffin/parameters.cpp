#include "PlasX/Falciparum/Griffin/parameters.h"
#include <PlasX/udl.hpp>
namespace plasx {
namespace falciparum {
namespace griffin {
Parameters::Parameters() {
  // To do, go through and check the units of everything.
  // All of the units are in days, therefore EIR is infections per bite per day,
  // not year. Death rate (1/average_age)
  mu_d = 0.0/ 12.0_yrs;
  // We probably want to build a parameters structure that reads in a file of
  // parameters.
  sigma = 1.27;
  // sigma = 0.01;

  age_0 = 2920.0_days;
  rho = 0.85;
  r_T = 1.0 / 5.0_days;     // This should be (1/days)
  r_D = 1.0 / 5.0_days;     // This should be (1/days)
  r_U = 1.0 / 168.0_days;   // This should be (1/days)
  r_A0 = 1.0 / 200.0_days;  // This should be (1/days)
  r_P = 1.0 / 25.0_days;    // This should be (1/days)
  f_T = 0.2;           // This is a proportion.

  // Bite parameters.
  b_min = 0.005;
  b_max = 0.89;

  // Immunity decay rates.
  d_A = 10.0_yrs;  // Should be in days.
  d_B = 10.0_yrs;  // Days.
  d_C = 30.0_days;          // Days.
  d_M = 255.5_days;         // Days.

  // Maternal immunity.
  P_M = 0.0;  // Proportion of maternal immunity

  // Kappa parameters.
  kappa_A = 5.0;  // Exponents.
  kappa_B = 4.93;
  kappa_C = 4.13;

  // Base immunities.
  I_A0 = 4732.5;
  I_B0 = 1.0;
  I_C0 = 1.0;

  // This one.
  w_A = 1.0;
}

}  // namespace griffin
}  // namespace falciparum
}  // namespace plasx