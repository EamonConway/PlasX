#include "PlasX/Vivax/White/mosquito_parameters.hpp"
namespace plasx {
namespace vivax {
namespace white {
MosquitoParameters::MosquitoParameters(const nlohmann::json &json)
    : beta(json.at("eggs_laid_per_female_mosquito").get<RealType>()),
      mu_E0(json.at("death_rate_early_instars").get<RealType>()),
      d_e(json.at("development_early_larval_instars").get<RealType>()),
      mu_l0(json.at("death_rate_late_instars").get<RealType>()),
      gamma(json.at("gamma").get<RealType>()),
      dl(json.at("development_late_larval_instars").get<RealType>()),
      dp(json.at("development_pupae").get<RealType>()),
      mu_p(1.0 / json.at("death_rate_pupae").get<RealType>()),
      mu_m(1.0 / json.at("life_expectancy").get<RealType>()),
      mu_tau(1.0 / json.at("sporogony_duration").get<RealType>()) {}
}  // namespace white
}  // namespace vivax
}  // namespace plasx