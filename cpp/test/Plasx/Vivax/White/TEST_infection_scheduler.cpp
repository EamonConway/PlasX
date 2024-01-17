#include "PlasX/Vivax/White/infection_scheduler.hpp"
#include "gtest/gtest.h"
using namespace plasx::vivax::white;
TEST(InfectionSchedule, ConstructorAndFunctions) {
  InfectionScheduler schedule;
  EXPECT_TRUE(schedule.empty());
  schedule.emplace(1.0, 1);
  schedule.emplace(2.0, 2);
  schedule.emplace(1.5, 2);
  EXPECT_EQ(schedule.top().getInfectionTime(), 1.0);
  EXPECT_EQ(schedule.top().getNumHypnozoites(), std::size_t(1));
  EXPECT_EQ(schedule.size(), std::size_t(3));
  schedule.pop();
  EXPECT_EQ(schedule.top().getInfectionTime(), 1.5);
  EXPECT_EQ(schedule.top().getNumHypnozoites(), std::size_t(2));
  EXPECT_EQ(schedule.size(), std::size_t(2));
  EXPECT_FALSE(schedule.empty());
  schedule.pop();
  EXPECT_FALSE(schedule.empty());
  schedule.pop();
  EXPECT_TRUE(schedule.empty());
}
