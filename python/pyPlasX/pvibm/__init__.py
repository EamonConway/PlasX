"""A Plasmodium Vivax Individual Based  Model.

Contained within this submodule is a python implementation of the Plasmodium
Vivax Individual based model(PVIBM) of White et al(2014). The states that an
individual can occupy are also exposed through the status class.

Typical usage example:
"""
from . import status_ as status
from . import parameters_ as parameters
from . import equilibrium_ as equilibrium
