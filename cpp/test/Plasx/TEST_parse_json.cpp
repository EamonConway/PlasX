#include "PlasX/parse_json.hpp"
#include "gtest/gtest.h"

namespace {
constexpr auto json_file = R"json({
"num_people": 2000
})json";

constexpr auto broken_json_file = R"json({
"num_people": 2000,
})json";
}  // namespace

TEST(PlasX, parse_json) {
  auto json = plasx::parse_json(json_file);
  EXPECT_TRUE(json.has_value());
  auto broken_json = plasx::parse_json(broken_json_file);
  EXPECT_FALSE(broken_json.has_value());
  EXPECT_THROW({ throw broken_json.error(); }, std::runtime_error);
}
