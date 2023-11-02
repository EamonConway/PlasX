/**
 * @file parameters.hpp
 * @author Eamon Conway (conway.e@wehi.edu.au)
 * @brief
 * @version 0.1
 * @date 2023-06-20
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef PLASX_VIVAX_WHITE_PARAMETERS_HPP
#define PLASX_VIVAX_WHITE_PARAMETERS_HPP
#include "nlohmann/json.hpp"
namespace plasx {
namespace vivax {
namespace white {
class Parameters {
 private:
 public:
  /**
   * @brief Construct a new Parameters object
   *
   * @param filename
   */
  Parameters(const nlohmann::json &json);

  Parameters(int num_people, double delay, double min_birth_age,
             double max_birth_age, double mu_d, double f, double gamma,
             double age_0, double rho, double r_P, double r_T, double r_D,
             double r_LM, double phiLM_min, double phiLM_50, double phiLM_max,
             double kappa_LM, double phiD_min, double phiD_50, double phiD_max,
             double kappa_D, double chiT, double dPCR_min, double dPCR_max,
             double dPCR_50, double kappa_PCR, double b,
             double duration_parasite_immunity,
             double duration_clinical_immunity,
             double duration_maternal_immunity,
             double proportion_maternal_immunity, double end_maternal_immunity,
             double refractory_period, double c_ILM, double c_IPCR, double c_ID,
             double c_T, double biting_rate_log_mean, double biting_rate_log_sd,
             double max_age);

  struct Compressed {
    double delay;
    double min_birth_age;
    double max_birth_age;

    // Death rate (1/average age)
    double mu_d;

    // Hypnozoite parameters.
    double f;
    double gamma;

    // Constant parameters. Will not change in a simulation.
    double age_0;
    double rho;
    double r_P;
    double r_T;
    double r_D;
    double r_LM;

    // Infection parameters - splitting.
    double phiLM_min, phiLM_50, phiLM_max, kappa_LM;
    double phiD_min, phiD_50, phiD_max, kappa_D;

    // Treatment parameter
    double chiT;

    // PCR duration parameters.
    double dPCR_min, dPCR_max, dPCR_50, kappa_PCR;

    // Bite parameters.
    double b;

    // Immunity decay rates.
    //    double exp_rate_dt_parasite_immunity, exp_rate_dt_clinical_immunity,
    //        exp_rate_dt_maternal_immunity;

    double duration_parasite_immunity, duration_clinical_immunity,
        duration_maternal_immunity;

    // Maternal immunity.
    double proportion_maternal_immunity;
    double end_maternal_immunity;

    // Refractory
    double refractory_period;

    double c_ILM;
    double c_IPCR;
    double c_ID;
    double c_T;

    // Bite heterogeneity
    double biting_rate_log_mean;
    double biting_rate_log_sd;

    // Maximum age parameters.
    double max_age;
  };

  Compressed compressedOutput() const {
    return {delay,
            min_birth_age,
            max_birth_age,
            mu_d,
            f,
            gamma,
            age_0,
            rho,
            r_P,
            r_T,
            r_D,
            r_LM,
            phiLM_min,
            phiLM_50,
            phiLM_max,
            kappa_LM,
            phiD_min,
            phiD_50,
            phiD_max,
            kappa_D,
            chiT,
            dPCR_min,
            dPCR_max,
            dPCR_50,
            kappa_PCR,
            b,
            duration_parasite_immunity,
            duration_clinical_immunity,
            duration_maternal_immunity,
            proportion_maternal_immunity,
            end_maternal_immunity,
            refractory_period,
            c_ILM,
            c_IPCR,
            c_ID,
            c_T,
            biting_rate_log_mean,
            biting_rate_log_sd,
            max_age};
  }

  int num_people;

  //
  double delay;
  double min_birth_age;
  double max_birth_age;

  // Death rate (1/average age)
  double mu_d;

  // Hypnozoite parameters.
  double f;
  double gamma;

  // Constant parameters. Will not change in a simulation.
  double age_0;
  double rho;
  double r_P;
  double r_T;
  double r_D;
  double r_LM;

  // Infection parameters - splitting.
  double phiLM_min, phiLM_50, phiLM_max, kappa_LM;
  double phiD_min, phiD_50, phiD_max, kappa_D;

  // Treatment parameter
  double chiT;

  // PCR duration parameters.
  double dPCR_min, dPCR_max, dPCR_50, kappa_PCR;

  // Bite parameters.
  double b;

  // Immunity decay rates.
  //  double exp_rate_dt_parasite_immunity, exp_rate_dt_clinical_immunity,
  //      exp_rate_dt_maternal_immunity;
  double duration_parasite_immunity, duration_clinical_immunity,
      duration_maternal_immunity;
  // Maternal immunity.
  double proportion_maternal_immunity;
  double end_maternal_immunity;

  // Refractory
  double refractory_period;

  double c_ILM;
  double c_IPCR;
  double c_ID;
  double c_T;

  // Bite heterogeneity
  double biting_rate_log_mean;
  double biting_rate_log_sd;

  // Maximum age parameters.
  double max_age;
};
}  // namespace white
}  // namespace vivax
}  // namespace plasx
#endif
