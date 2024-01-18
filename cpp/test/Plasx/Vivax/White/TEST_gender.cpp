#include "PlasX/Vivax/White/gender.hpp"
#include "gtest/gtest.h"

using plasx::vivax::white::Gender;
TEST(Gender, Enum) {
  auto female_at_birth = Gender::AFAB;
  auto male_at_birth = Gender::AMAB;
  EXPECT_FALSE(female_at_birth==male_at_birth);
  EXPECT_TRUE(female_at_birth!=male_at_birth);
}
