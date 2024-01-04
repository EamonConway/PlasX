#include "PlasX/Mosquito/simple_mosquito_ode_fn.hpp"
namespace plasx {
namespace mosquito {
std::array<RealType, 3> simple_mosquito_ode_fn::operator()(
    const RealType , const std::array<RealType, 3>& state,
    const RealType lambda,
    const mosquito::SimpleMosquitoParameters& params) const {
  auto& [S, E, I] = state;
  auto& [mu_0, gamma, zeta, phi] = params;
  const auto mu = mu_0;
  const auto m = S + E + I;
  const auto dS = mu_0 * m - lambda * S - mu * S;
  const auto dE = lambda * S - (gamma + mu) * E;
  const auto dI = gamma * E - mu * I;
  return {dS, dE, dI};
}
}  // namespace mosquito
}  // namespace plasx
