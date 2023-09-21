#ifndef PLASX_MODEL_SIMULATION_FN_HPP
#define PLASX_MODEL_SIMULATION_FN_HPP
#include <tuple>

#include "PlasX/details/function_args.hpp"
#include "PlasX/types.hpp"
namespace plasx {
template <typename HumanModelType, typename MosquitoModelType>
struct model_simulation_fn {
  constexpr model_simulation_fn(HumanModelType&& human,
                                MosquitoModelType&& mosquito)
      : human_model_fn(std::forward<HumanModelType>(human)),
        mosquito_model_fn(std::forward<MosquitoModelType>(mosquito)){};

  template <typename Eir, typename... ModelArgs>
  auto operator()(const RealType t0, const RealType t1, const RealType dt,
                  Eir&& initial_eir, ModelArgs&&... model_args) {
    // Split the arguments into the human and mosquito model.
    constexpr auto num_human_args =
        FunctionArgumentCounter<HumanModelType>::value - 3;
    auto [human_args, mosquito_args] =
        splitHumanMosquitoArguments<num_human_args>{}(
            std::forward<ModelArgs>(model_args)...);

    // Initial condition does not have to satisfy the model equations.
    auto t = t0;
    // In a perfect world I would use structure bindings for the return type of
    // human_model_fn and mosquito_model_fn. However, structured bindings cannot
    // be captured by reference in lambda functions with Clang. This makes the
    // code less readable in my opinion, but is worth ensuring that there are no
    // compile errors on different machines.
    auto eir = initial_eir;
    while (t < t1) {
      // Calculate the mosquito to human interaction.
      auto human_output = std::apply(
          [&](auto&&... args) {
            return human_model_fn(t, dt, eir,
                                  std::forward<decltype(args)>(args)...);
          },
          human_args);
      // Calculate the human mosquito interaction.
      auto mosquito_output = std::apply(
          [&](auto&&... args) {
            return mosquito_model_fn(t, dt, human_output.second,
                                     std::forward<decltype(args)>(args)...);
          },
          mosquito_args);

      eir = mosquito_output.second;
      [[maybe_unused]] auto combined_output =
          std::make_pair(human_output, mosquito_output);
      t += dt;
    }
  };

 private:
  HumanModelType human_model_fn;
  MosquitoModelType mosquito_model_fn;
};
}  // namespace plasx
#endif
