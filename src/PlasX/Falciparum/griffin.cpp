#include "PlasX/Falciparum/griffin.hpp"

#include "PlasX/random.hpp"

namespace plasx {
namespace falciparum {
namespace griffin {
// Construct the object that will store the information in the Griffin
// simulation.
PFalc::PFalc(const Status& status, double ICA, double ICM, double IA)
    : current_(status),
      I_CA_(ICA),
      I_CM_(ICM),
      I_A_(IA),
      cached_infection_(std::numeric_limits<double>::infinity()){};

double PFalc::getIC() noexcept { return I_CA_ + I_CM_; }

double PFalc::getIA() noexcept { return I_A_; }

void PFalc::clearInfectionQueue() {
  // Replace with an empty queue.
  infection_queue_ =
      std::priority_queue<double, std::vector<double>, std::less<double>>();
}

void PFalc::scheduleInfection(const double t) {
  const auto isCacheable = t < cached_infection_;
  // You have to do the push first. or you might overright the cached value and
  // lose it
  infection_queue_.push(!isCacheable * t + isCacheable * cached_infection_);
  cached_infection_ = isCacheable * t + !isCacheable * cached_infection_;
  // Arguably faster than having the if statements.... will have to check.
};

bool PFalc::updateInfection(const double t) {
  // Technically, by using a priority queue there is an indirection to the
  // stack as all data is held in a vector. We could consider caching the top
  // value in the individual structure and only go away once that value is
  // outdated. If the infectionQueue is empty, the next infection could be at
  // time infinity. We would have to make sure that we update things
  // appropriately for that however. This may not be the best, at a new
  // infection we would have to check against cache value. If cahced value is
  // larger, we need to move it bak into the queue. But this might just be fine.
  // It is an interesting thing to look for.
  if (t >= cached_infection_) [[unlikely]] {
    // The cached infection is going to take place.
    if (infection_queue_.empty()) {
      cached_infection_ = std::numeric_limits<double>::infinity();
    } else {
      cached_infection_ = infection_queue_.top();
      infection_queue_.pop();
    }
    return true;
  } else [[likely]] {
    return false;
  }
}

}  // namespace griffin
}  // namespace falciparum
}  // namespace plasx