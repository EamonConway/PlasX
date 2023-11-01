#ifndef PLASX_MOSQUITO_MOSQUITO_PARAMETERS_HPP
#define PLASX_MOSQUITO_MOSQUITO_PARAMETERS_HPP
#include "PlasX/types.hpp"
#include "nlohmann/json.hpp"
namespace plasx {
namespace mosquito {
class MosquitoParameters {
 public:
  MosquitoParameters(const nlohmann::json &json);
  RealType beta;
  RealType mu_E0;
  RealType d_e;
  RealType mu_l0;
  RealType gamma;
  RealType dl;
  RealType dp;
  RealType mu_p;
  RealType mu_m;
  RealType mu_tau;
};
}  // namespace mosquito
}  // namespace plasx
#endif
