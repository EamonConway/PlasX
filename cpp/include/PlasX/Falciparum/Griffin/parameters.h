#ifndef PLASX_FALCIPARUM_GRIFFIN_PARAMETERS_H
#define PLASX_FALCIPARUM_GRIFFIN_PARAMETERS_H

namespace plasx {
namespace falciparum {
namespace griffin {
class Parameters {
 private:
 public:
  Parameters();  // Constructor function. Will probably just read in parameters
                 // from file.

  // Death rate (1/average age)
  double mu_d;

  // Constant parameters. Will not change in a simulation.
  double sigma;
  double age_0;
  double rho;
  double r_T;
  double r_D;
  double r_U;
  double r_A0;
  double r_P;
  double f_T;

  // Bite parameters.
  double b_min;
  double b_max;

  // Immunity decay rates.
  double d_A;
  double d_B;
  double d_C;
  double d_M;

  // Maternal immunity.
  double P_M;

  // Kappa parameters.
  double kappa_A;
  double kappa_B;
  double kappa_C;

  // Initial immunities.
  double I_A0;
  double I_B0;
  double I_C0;

  // This one.
  double w_A;

  // Currently a class (possibility that we are sampling here).
};

}  // namespace griffin
}  // namespace falciparum
}  // namespace plasx
#endif