#include "PlasX/Vivax/White/mosquito.hpp"

#include <array>
#include <queue>

#include "PlasX/types.hpp"
namespace plasx {
namespace vivax {
namespace white {
std::array<RealType, 6> MosquitoModel::operator()(
    const RealType t, const std::array<RealType, 6>& state,
    const RealType lambda, const RealType kappa,
    const MosquitoParameters& params) const {
  auto &beta = params.beta, &mu_E0 = params.mu_E0, &d_e = params.d_e,
       &mu_l0 = params.mu_l0, &gamma = params.gamma, &dl = params.dl,
       &dp = params.dp, &mu_p = params.mu_p, &mu_m = params.mu_m,
       &mu_tau = params.mu_tau;
  // State reference to easier to interpret.
  auto &Le = state[0], &Ll = state[1], &Lp = state[2], &S = state[3],
       &E = state[4], &I = state[5];

  // Calculated parameters.
  auto m_total = S + E + I;
  const auto instar_larvae_seasonality = (Le + Ll) / kappa;

  // This is going to be the value that has been calculated before with the
  // lag.
  auto lagged_value = mu_tau * E;
  // Equation system.
  const auto dLe =
      beta * m_total - mu_E0 * (1 + instar_larvae_seasonality) * Le - Le / d_e;
  const auto dLl =
      Le / d_e - mu_l0 * (1 + gamma * instar_larvae_seasonality) * Ll - Ll / dl;
  const auto dLp = Ll / dl - mu_p * Lp - Lp / dp;
  const auto dS = 0.5 * Lp / dp - lambda * S - mu_m * S;
  const auto dE = lambda * S - mu_m * E - lagged_value;
  const auto dI = lagged_value - mu_m * I;

  return {dLe, dLl, dLp, dS, dE, dI};
};
}  // namespace white
}  // namespace vivax
}  // namespace plasx