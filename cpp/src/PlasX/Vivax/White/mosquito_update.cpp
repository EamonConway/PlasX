#include "PlasX/Vivax/White/mosquito_update.hpp"

#include "PlasX/Mosquito/multispecies_mosquito_model.hpp"
#include "PlasX/Mosquito/simple_mosquito_ode_fn.hpp"
#include "PlasX/Mosquito/simple_mosquito_parameters.hpp"
namespace plasx {
namespace vivax {
namespace white {
auto simple_mosquito_update(
    double dt, double t0, double lambda,
    const std::unordered_map<plasx::MosquitoSpecies, std::array<RealType, 3>>&
        initial_state,
    const std::unordered_map<plasx::MosquitoSpecies,
                             mosquito::SimpleMosquitoParameters>&
        mosquito_parameters)
    -> std::pair<
        std::unordered_map<plasx::MosquitoSpecies, std::array<RealType, 3>>,
        RealType> {
  using plasx::mosquito::mosquito_ode_model;
  using plasx::mosquito::simple_mosquito_ode;
  using plasx::mosquito::SimpleMosquitoParameters;

  auto CalculateFoi = [](const std::array<RealType, 3>& state,
                         const SimpleMosquitoParameters&) { return state[2]; };

  return mosquito_ode_model(t0, dt, lambda, CalculateFoi, simple_mosquito_ode,
                            initial_state, mosquito_parameters);
};
}  // namespace white
}  // namespace vivax
}  // namespace plasx
