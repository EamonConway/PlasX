from pybin.pyPlasX.mosquito import SimpleMosquitoParams as Parameters
from pybin.pyPlasX.mosquito import mosquito_model
import matplotlib.pyplot as plt
import numpy as np

parameters = Parameters(death_rate=1.0/20.0, zeta=1.0, phi=1.0, gamma=1.0/12.0)

t, y = mosquito_model(0.01, 0.125, 0.0, 500.0, [0.99, 0.01, 0.0],  parameters)

y_matrix = np.array(y)
plt.plot(t, y_matrix)
plt.show()
