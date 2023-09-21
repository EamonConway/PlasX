#ifndef PLASX_MODEL_SIMULATION_FN_HPP
#define PLASX_MODEL_SIMULATION_FN_HPP
#include <tuple>
#include <vector>

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
    // Initial condition does not have to satisfy the model equations.
    auto t = t0;
    auto eir = initial_eir;

    // Split model_args into the two seperate tuples of arguments.
    constexpr auto num_human_args =
        FunctionArgumentCounter<HumanModelType>::value - 3;
    auto [human_args, mosquito_args] =
        splitHumanMosquitoArguments<num_human_args>{}(
            std::forward<ModelArgs>(model_args)...);

    using HumanModelReturnType = decltype(std::apply(
        human_model_fn, std::tuple_cat(std::tuple(t, dt, eir), human_args)));

    using MosquitoModelReturnType = decltype(std::apply(
        mosquito_model_fn,
        std::tuple_cat(
            std::tuple(t, dt, std::declval<HumanModelReturnType>().second),
            mosquito_args)));

    using OutputTypes =
        std::tuple<RealType, HumanModelReturnType, MosquitoModelReturnType>;

    // Output declaration
    std::vector<OutputTypes> output;

    while (t <= t1) {
      // Calculate the mosquito to human interaction.
      HumanModelReturnType human_output = std::apply(
          [&](auto&&... args) {
            return human_model_fn(t, dt, eir,
                                  std::forward<decltype(args)>(args)...);
          },
          human_args);
      // Calculate the human mosquito interaction.
      MosquitoModelReturnType mosquito_output = std::apply(
          [&](auto&&... args) {
            return mosquito_model_fn(t, dt, human_output.second,
                                     std::forward<decltype(args)>(args)...);
          },
          mosquito_args);

      eir = mosquito_output.second;

      output.emplace_back(std::make_tuple(t, human_output, mosquito_output));
      t += dt;
    }
    return output;
  };

 private:
  HumanModelType human_model_fn;
  MosquitoModelType mosquito_model_fn;
};
}  // namespace plasx
#endif
