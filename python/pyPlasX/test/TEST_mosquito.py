import pyPlasX

# Create the state vector.
state = {"species_one": [0.99, 0.01, 0.0],
         "species_two": [0.99, 0.01, 0.0],
         "species_three": [0.99, 0.01, 0.0]}

# Create the parameters

params = {
    "species_one": pyPlasX.SimpleMosquitoParameters(
        death_rate=1.0, gamma=12.0, zeta=1.0, phi=1.0),
    "species_two": pyPlasX.SimpleMosquitoParameters(
        death_rate=1.23401, gamma=1.120, zeta=1.110, phi=1.10),
    "species_three": pyPlasX.SimpleMosquitoParameters(
        death_rate=1.23401, gamma=1.120, zeta=1.110, phi=1.10)
}

# Run a simulation
t, y = pyPlasX.ms_mosquito_model(0.1, 0.125, 0.0, 1.0, state, params)
t1, y1 = pyPlasX.mosquito_model(0.1, 0.125, 0.0, 1.0,
                        state["species_one"], params["species_one"])
t2, y2 = pyPlasX.mosquito_model(0.1, 0.125, 0.0, 1.0,
                        state["species_two"], params["species_two"])

# Check if the results match.
print([x[0]["species_one"] for x in y])
print([x[0]["species_two"] for x in y])
print(y1)
