#ifndef PLASX_VIVAX_WHITE_MULTISPECIES_MOSQUITO_MODEL_HPP
#define PLASX_VIVAX_WHITE_MULTISPECIES_MOSQUITO_MODEL_HPP
#include <type_traits>
#include <unordered_map>

#include "PlasX/types.hpp"
#include "odepp/integrators/forward_euler.hpp"
namespace plasx {
namespace mosquito {

template <typename Fn, typename ModelState, typename ModelParameters>
concept FoiConcept =
    std::is_invocable_r_v<RealType, Fn, ModelState, ModelParameters>;

template <typename Model, typename ModelState, typename ModelParameters>
concept MosquitoOdeConcept =
    std::is_invocable_r_v<ModelState, Model, RealType, ModelState, RealType,
                          ModelParameters>;

struct MultiSpeciesMosquitoOdeFn {
  template <typename ModelState, typename ModelParameters,
            MosquitoOdeConcept<ModelState, ModelParameters> Model,
            FoiConcept<ModelState, ModelParameters> FoiFn>
  [[nodiscard("MultiSpeciesMosquitoOdeFn")]] auto operator()(
      RealType& t, RealType dt, RealType lambda, FoiFn&& getFoi, Model&& model,
      const std::unordered_map<MosquitoSpecies, ModelState>& state,
      const std::unordered_map<MosquitoSpecies, ModelParameters>& params)
      const {
    // Run the mosquito model over all mosquito species in the unordered_map
    auto UpdateSingleSpecies = [&](const auto& species,
                                   const auto& species_state, auto t) {
      return odepp::integrator::forward_euler(model, dt, t, species_state,
                                              lambda, params.at(species));
    };

    auto UpdateMosquitoes = [&](const auto& mosquitoes) {
      std::pair<std::unordered_map<MosquitoSpecies, ModelState>, RealType>
          output;
      for (const auto& [species, state] : mosquitoes) {
        auto [state_it, inserted] = output.first.emplace(
            species, UpdateSingleSpecies(species, state, t));
        output.second += getFoi(state_it->second, params.at(species));
      }
      t += dt;
      return output;
    };
    return UpdateMosquitoes(state);
  };
};

inline constexpr MultiSpeciesMosquitoOdeFn mosquito_ode_model{};
}  // namespace mosquito
}  // namespace plasx
#endif  // !PLASX_VIVAX_WHITE_MULTISPECIES_MOSQUITO_MODEL
