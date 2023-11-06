#ifndef PLASX_VIVAX_WHITE_MULTISPECIES_MOSQUITO_MODEL_HPP
#define PLASX_VIVAX_WHITE_MULTISPECIES_MOSQUITO_MODEL_HPP
#include <concepts>
#include <numeric>
#include <type_traits>
#include <unordered_map>

#include "PlasX/types.hpp"
#include "odepp/integrators/forward_euler.hpp"
namespace plasx {
namespace mosquito {
// Ensure that Mosquito Odes satisfy the same criteria.
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
  auto operator()(RealType t, RealType dt, RealType lambda, FoiFn&& getFoi,
                  Model&& model,
                  const std::unordered_map<MosquitoSpecies, ModelState>& state,
                  const std::unordered_map<MosquitoSpecies, ModelParameters>&
                      params) const {
    auto output_state = state;

    const auto AccumulateFoi = [&getFoi, &params](const auto lhs,
                                                  const auto& rhs) {
      const auto& [species, species_state] = rhs;
      return lhs + getFoi(species_state, params.at(species));
    };

    const auto UpdateMosquitoes = [&model, &dt, &t, &lambda,
                                   &params](const auto& mosquito) {
      const auto& [species, species_state] = mosquito;
      return odepp::integrator::forward_euler(model, dt, t, species_state,
                                              lambda, params.at(species));
    };

    auto foi = std::transform_reduce(std::begin(state), std::end(state),
                                     std::begin(output_state), 0.0,
                                     UpdateMosquitoes, AccumulateFoi);

    return std::pair(output_state, foi);
  }
};

inline constexpr MultiSpeciesMosquitoOdeFn mosquito_ode_model{};
}  // namespace mosquito
}  // namespace plasx
#endif  // !PLASX_VIVAX_WHITE_MULTISPECIES_MOSQUITO_MODEL
