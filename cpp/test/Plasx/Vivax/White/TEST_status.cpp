#include <cstdint>

#include "PlasX/Vivax/White/status.hpp"
#include "gtest/gtest.h"
using namespace plasx::vivax::white;

TEST(PVivaxStatus, IntCast) {
  EXPECT_EQ(static_cast<uint8_t>(Status::S), 0);
  EXPECT_EQ(static_cast<uint8_t>(Status::I_LM), 1);
  EXPECT_EQ(static_cast<uint8_t>(Status::I_PCR), 2);
  EXPECT_EQ(static_cast<uint8_t>(Status::I_D), 3);
  EXPECT_EQ(static_cast<uint8_t>(Status::T), 4);
  EXPECT_EQ(static_cast<uint8_t>(Status::P), 5);
}
