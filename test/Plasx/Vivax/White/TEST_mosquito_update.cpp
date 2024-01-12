#include "PlasX/Mosquito/simple_mosquito_parameters.hpp"
#include "PlasX/Vivax/White/mosquito_update.hpp"
#include "PlasX/types.hpp"
#include "gtest/gtest.h"
using namespace plasx::vivax::white;
using namespace plasx;
using plasx::MosquitoSpecies;
using plasx::mosquito::SimpleMosquitoParameters;
using Parameters =
    std::unordered_map<MosquitoSpecies, SimpleMosquitoParameters>;
using State =
    std::unordered_map<plasx::MosquitoSpecies, std::array<RealType, 3>>;

TEST(MosquitoUpdate, SimpleUpdater) {
  auto parameters = Parameters{{"first", {1.0, 1.0, 1.0, 1.0}}};
  auto state = State{{"first", {9.0, 1.0, 0.0}}};
  auto [new_state, foi] =
      simple_mosquito_update(0.25, 0.0, 1.0, state, parameters);
  for (const auto &[_, x] : new_state) {
    for (const auto &y : x) {
      std::cout << y << std::endl;
    }
  }
}

TEST(MosquitoUpdate, MultiUpdater) {
  auto parameters = Parameters{{"first", {1.0, 1.0, 1.0, 1.0}},
                               {"second", {1.0, 1.0, 1.0, 1.0}}};
  auto state = State{{"first", {9.0, 1.0, 0.0}}, {"second", {9.0, 1.0, 0.0}}};
  auto [new_state, foi] =
      simple_mosquito_update(0.25, 0.0, 1.0, state, parameters);
  auto first = new_state.at("first");
  for (const auto &[_, x] : new_state) {
    EXPECT_EQ(first, x);
  }
}
