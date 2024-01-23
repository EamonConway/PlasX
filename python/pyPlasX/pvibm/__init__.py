"""A Plasmodium Vivax Individual Based  Model.

Contained within this submodule is a python implementation of the Plasmodium
Vivax Individual based model(PVIBM) of White et al(2014). The states that an
individual can occupy are also exposed through the status class.

Typical usage example:
"""
from pyPlasX.pvibm import _equilibrium as equilibrium
from pyPlasX.pvibm import _model as model
from pyPlasX.pvibm import _parameters as parameters
from pyPlasX.pvibm import _population as population
from pyPlasX.pvibm import _status as status
