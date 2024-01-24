from pyPlasX import pvibm
import matplotlib.pyplot as plt

parameters = pvibm.HumanParameters(num_people=10000,
                        delay=10.0,
                        maternal_min_age=6570.0,
                        maternal_max_age=14600.0,
                        life_expectancy=8212.5,
                        time_to_relapse=41.0,
                        time_to_clear_hypnozoite=383.0,
                        age_0=2920.0,
                        rho=0.85,
                        duration_prophylaxis=5.0,
                        duration_treatment=1.0,
                        duration_high_density_infection=5.0,
                        duration_light_microscopy=16.0,
                        phi_LM_min=0.011,
                        phi_LM_50=18.8,
                        phi_LM_max=0.93,
                        kappa_LM=3.37,
                        phiD_min=0.006,
                        phiD_50=24.5,
                        phiD_max=0.96,
                        kappa_D=5.63,
                        chiT=0.5,
                        dPCR_min=10.0,
                        dPCR_50=9.9,
                        dPCR_max=52.6,
                        kappa_PCR=3.82,
                        b=0.25,
                        duration_parasite_immunity=3650.0,
                        duration_clinical_immunity=10950.0,
                        duration_maternal_immunity=49.9,
                        proportion_maternal_immunity=0.31,
                        end_maternal_immunity=365.0,
                        refractory_period=42.4,
                        c_ILM=0.1,
                        c_IPCR=0.035,
                        c_ID=0.8,
                        c_T=0.4,
                        biting_rate_log_mean=0.0,
                        biting_rate_log_sd=1.10905365064,
                        max_age=29200.0
                        )

# t, y = pvibm.run_equilibrium(
#     0.0, 20.0*365.0, 1.0, 2.0*parameters.num_people, parameters)

# for status, prevalence in y.items():
#     plt.plot(t, prevalence, label=f"Status {status}")
# plt.show()
