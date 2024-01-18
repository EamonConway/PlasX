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
  [[nodiscard("model_simulation_fn")]] auto operator()(
      const RealType t0, const RealType t1, const RealType dt,
      Eir&& initial_eir, ModelArgs&&... model_args) const {
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
        human_model_fn,
        std::tuple_cat(std::forward_as_tuple(t, dt, eir), human_args)));

    using MosquitoModelReturnType = decltype(std::apply(
        mosquito_model_fn,
        std::tuple_cat(std::forward_as_tuple(
                           t, dt, std::declval<HumanModelReturnType>().second),
                       mosquito_args)));

    static_assert(
        !std::is_same_v<Eir, typename MosquitoModelReturnType::second_type>,
        "initial_eir is required to have the same type as "
        "MosquitoModelReturnType::second_type");

    // Output declaration
    auto time_output_store = std::vector<RealType>();
    auto human_state_output =
        std::vector<typename HumanModelReturnType::first_type>();
    auto mosquito_state_store =
        std::vector<typename MosquitoModelReturnType::first_type>();

    while (t <= t1) {
      // Calculate the mosquito to human interaction.
      HumanModelReturnType human_output = std::apply(
          human_model_fn,
          std::tuple_cat(std::forward_as_tuple(t, dt, eir), human_args));

      // Calculate the human mosquito interaction.
      MosquitoModelReturnType mosquito_output = std::apply(
          mosquito_model_fn,
          std::tuple_cat(std::forward_as_tuple(t, dt, human_output.second),
                         mosquito_args));

      eir = mosquito_output.second;
      time_output_store.emplace_back(t);
      human_state_output.emplace_back(human_output.first);
      mosquito_state_store.emplace_back(mosquito_output.first);
      t += dt;
    }
    return std::make_tuple(time_output_store, human_state_output,
                           mosquito_state_store);
  }

 private:
  HumanModelType human_model_fn;
  MosquitoModelType mosquito_model_fn;
};
}  // namespace plasx
#endif
