import pandas as pd
import numpy as np
import sys
import matplotlib.pyplot as plt
sys.path.append(r'/Users/conway.e/repos/PlasX/pybin')
import pyPlasX as px

t,y = px.vivax.white.mosquito_model(1.0,1000000.0,0.125,0.0,400.0,{
  "eggs_laid_per_female_mosquito": 21.19,
  "development_early_larval_instars": 6.64,
  "death_rate_early_instars": 0.034,
  "development_late_larval_instars": 3.72,
  "death_rate_late_instars": 0.035,
  "gamma": 13.25,
  "development_pupae": 0.64,
  "death_rate_pupae": 0.25,
  "life_expectancy": 6.0,
  "sporogony_duration": 9.4
})

x = np.array(t)
f = np.array(y)

plt.plot(x,f[:,3::5])
plt.show()