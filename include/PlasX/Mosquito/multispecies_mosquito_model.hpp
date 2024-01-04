#ifndef PLASX_VIVAX_WHITE_MULTISPECIES_MOSQUITO_MODEL_HPP
#define PLASX_VIVAX_WHITE_MULTISPECIES_MOSQUITO_MODEL_HPP
#include <numeric>
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
    auto UpdateMosquitoes = [&](const auto& mosquito) {
      const auto& [species, species_state] = mosquito;
      auto local_t = t;
      auto updated_species_state = odepp::integrator::forward_euler(
          model, dt, local_t, species_state, lambda, params.at(species));
      return getFoi(updated_species_state, params.at(species));
    };

    auto UpdateStateAppendTotalFOI = [&](const auto& state) {
      auto return_value = std::pair(state, 0.0);
      auto& [output_state, foi] = return_value;
      foi = std::transform_reduce(std::begin(output_state),
                                  std::end(output_state), 0.0, std::plus(),
                                  UpdateMosquitoes);
      return return_value;
    };

    auto output = UpdateStateAppendTotalFOI(state);
    t += dt;
    return output;
  }
};

inline constexpr MultiSpeciesMosquitoOdeFn mosquito_ode_model{};
}  // namespace mosquito
}  // namespace plasx
#endif  // !PLASX_VIVAX_WHITE_MULTISPECIES_MOSQUITO_MODEL
