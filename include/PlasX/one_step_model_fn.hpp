#ifndef PLASX_ONE_STEP_MODEL_FN_HPP
#define PLASX_ONE_STEP_MODEL_FN_HPP
#include <tuple>

#include "PlasX/types.hpp"
namespace plasx {
namespace {
template <unsigned N>
struct splitHumanMosquitoArguments {
  template <typename First, typename... Args>
  constexpr auto operator()(First&& f, Args&&... args) {
    auto value =
        splitHumanMosquitoArguments<N - 1>{}(std::forward<Args>(args)...);
    return std::pair{
        std::tuple_cat(std::forward_as_tuple(f), std::move(value.first)),
        std::move(value.second)};
  }

  constexpr auto operator()() { return splitHumanMosquitoArguments<N - 1>{}(); }
};

template <>
struct splitHumanMosquitoArguments<0> {
  template <typename... Args>
  constexpr auto operator()(Args&&... args) {
    return std::make_pair(std::forward_as_tuple(),
                          std::forward_as_tuple(args...));
  }
};

template <typename Func>
struct FunctionArgumentCount;

template <typename ReturnType, typename... Args>
struct FunctionArgumentCount<ReturnType(Args...)> {
  static constexpr std::size_t value = sizeof...(Args);
};

// Specialization for function pointers
template <typename ReturnType, typename... Args>
struct FunctionArgumentCount<ReturnType (*)(Args...)> {
  static constexpr std::size_t value = sizeof...(Args);
};
}  // namespace

template <class HumanModelType, class MosquitoModelType>
struct one_step_model_fn {
  constexpr one_step_model_fn(HumanModelType&& h, MosquitoModelType&& m)
      : human_model(h), mosquito_model(m){};
  template <typename... Args>
  auto operator()(RealType& t, RealType dt, Args&&... args) const {
    // Split the arguments into the human and mosquito model.
    constexpr auto N_human_args =
        FunctionArgumentCount<HumanModelType>::value - 3;
    constexpr auto N_mosquito_args =
        FunctionArgumentCount<MosquitoModelType>::value - 3;
    auto [human_args, mosquito_args] =
        splitHumanMosquitoArguments<N_human_args>{}(
            std::forward<Args>(args)...);

    // Calculate the mosquito to human interaction.
    const auto mosquito_human_interaction = 0.0;
    auto human_output = std::apply(
        [&](auto&&... args) {
          return human_model(t, dt, mosquito_human_interaction,
                             std::forward<decltype(args)>(args)...);
        },
        human_args);

    // Calculate the human mosquito interaction.
    const auto human_mosquito_interaction = 0.0;
    auto mosquito_output = std::apply(
        [&](auto&&... args) {
          return mosquito_model(t, dt, human_mosquito_interaction,
                                std::forward<decltype(args)>(args)...);
        },
        mosquito_args);

    return std::make_pair(human_output, mosquito_output);
  }

 private:
  HumanModelType human_model;
  MosquitoModelType mosquito_model;
};
}  // namespace plasx
#endif
