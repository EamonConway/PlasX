import pyPlasX
import matplotlib.pyplot as plt
import numpy as np

parameters = pyPlasX.SimpleMosquitoParameters(death_rate=1.0/20.0, zeta=1.0, phi=1.0, gamma=1.0/12.0)

t, y = pyPlasX.mosquito_model(0.201, 0.125, 0.0, 500.0, [0.99, 0.01, 0.0],  parameters)

y_matrix = np.array(y)
plt.plot(t, y_matrix)
plt.show()
