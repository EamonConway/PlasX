#ifndef PLASX_PARSE_JSON_HPP
#define PLASX_PARSE_JSON_HPP
#include <expected>

#include "nlohmann/json.hpp"
namespace plasx {
template <typename InputType>
std::expected<nlohmann::json, std::runtime_error> parse_json(
    InputType&& input) {
  try {
    return nlohmann::json::parse(std::forward<InputType>(input));
  } catch (const nlohmann::json::parse_error& e) {
    return std::unexpected(std::runtime_error(e.what()));
  } catch (...) {
    return std::unexpected(
        std::runtime_error("Unknown exception caught in plasx::parse_json"));
  }
}
}  // namespace plasx
#endif
