#ifndef PLASX_VIVAX_WHITE_INFECTIONSCHEDULER_HPP
#define PLASX_VIVAX_WHITE_INFECTIONSCHEDULER_HPP
#include <queue>

#include "PlasX/types.hpp"

namespace plasx {
namespace vivax {
namespace white {
class InfectionScheduler {
 public:
  InfectionScheduler(){};
  class Infection {
   public:
    Infection(RealType t, SizeType new_hypnozoites)
        : t_(t), hypnozoites_(new_hypnozoites){};

    friend bool operator<(const Infection& lhs, const Infection& rhs);

    RealType getInfectionTime() const noexcept { return t_; }
    SizeType getNumHypnozoites() const noexcept { return hypnozoites_; }

   protected:
   private:
    RealType t_;
    SizeType hypnozoites_;
  };

  using underlying_type = std::priority_queue<Infection, std::vector<Infection>,
                                             std::greater<Infection>>;

  void emplace(const RealType& t, const SizeType& hypnozoites) {
    q_.emplace(t, hypnozoites);
  }

  const Infection& top() const { return q_.top(); }
  bool empty() const { return q_.empty(); }
  void pop() { q_.pop(); }
  SizeType size() const { return q_.size(); };

 protected:
 private:
  underlying_type q_;
};

inline bool operator<(const InfectionScheduler::Infection& lhs,
                      const InfectionScheduler::Infection& rhs) {
  return lhs.t_ < rhs.t_;
}
inline bool operator>(const InfectionScheduler::Infection& lhs,
                      const InfectionScheduler::Infection& rhs) {
  return rhs < lhs;
}
}  // namespace white
}  // namespace vivax
}  // namespace plasx
#endif