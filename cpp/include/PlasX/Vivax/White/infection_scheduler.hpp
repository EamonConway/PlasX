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

    constexpr bool operator<(const Infection& rhs) const noexcept {
      return t_ < rhs.t_;
    }

    RealType getInfectionTime() const noexcept { return t_; }
    SizeType getNumHypnozoites() const noexcept { return hypnozoites_; }

   protected:
   private:
    RealType t_;
    SizeType hypnozoites_;
  };

  void emplace(const RealType& t, const SizeType& hypnozoites) {
    q_.emplace(t, hypnozoites);
  }

  const Infection& top() const { return q_.top(); }
  bool empty() const { return q_.empty(); }
  void pop() { q_.pop(); }
  SizeType size() const { return q_.size(); };

 protected:
 private:
  struct CompareInfections {
    bool operator()(const Infection& lhs, const Infection& rhs) const {
      return rhs < lhs;
    }
  };
  std::priority_queue<Infection, std::vector<Infection>, CompareInfections> q_;
};

}  // namespace white
}  // namespace vivax
}  // namespace plasx
#endif
