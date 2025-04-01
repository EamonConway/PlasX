"""A Plasmodium Vivax Individual Based  Model.

Contained within this submodule is a python implementation of the Plasmodium
Vivax Individual based model(PVIBM) of White et al(2014). The states that an
individual can occupy are also exposed through the status class.

Typical usage example:
"""

from ._status import Status as Status
from ._population import Population as Population
from ._parameters import HumanParameters as HumanParameters
from ._equilibrium import run as equilibrium_run
from ._model import run as simple_model_run
from numpy.random import lognormal
from numpy.random import exponential


def CreateInitialPopulation(params: HumanParameters):
    population = Population()
    age_samples = exponential(scale=params.death_rate, size=params.num_people)
    population_zeta = lognormal(
        params.biting_rate_log_mean, params.biting_rate_log_sd, params.num_people
    )
    for age, zeta in zip(age_samples, population_zeta):
        population.CreateIndividual(
            params.maternal_min_age,
            params.maternal_max_age,
            age,
            Status.S,
            0.0,
            0.0,
            0.0,
            0.0,
            zeta,
            params.rho,
            params.age_0,
            0,
        )
    return population
