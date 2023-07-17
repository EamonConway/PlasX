#include "PlasX/Vivax/White/status.hpp"
namespace plasx {
namespace vivax {
namespace white {
std::ostream& operator<<(std::ostream& os, const Status& data) {
  switch (data) {
    case Status::S:
      os << "S";
      break;
    case Status::I_LM:
      os << "I_LM";
      break;
    case Status::I_PCR:
      os << "I_PCR";
      break;
    case Status::I_D:
      os << "I_D";
      break;
    case Status::T:
      os << "T";
      break;
    case Status::P:
      os << "P";
      break;
    default:
      throw std::logic_error("Individual is in an unknown compartment.");
  }
  return os;
};
}  // namespace white
}  // namespace vivax
}  // namespace plasx