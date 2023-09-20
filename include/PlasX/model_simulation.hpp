#ifndef PLASX_MODEL_SIMULATION_FN_HPP
#define PLASX_MODEL_SIMULATION_FN_HPP
#include <tuple>
#include <type_traits>

#include "PlasX/Vivax/White/mosquito.hpp"
#include "PlasX/types.hpp"

namespace plasx {
namespace {

// functor
template<class F>
struct function_traits
{
    private:
        using call_type = function_traits<decltype(&F::operator())>;
    public:
        using return_type = typename call_type::return_type;

        static constexpr std::size_t arity = call_type::arity - 1;

        template <std::size_t N>
        struct argument
        {
            static_assert(N < arity, "error: invalid parameter index.");
            using type = typename call_type::template argument<N+1>::type;
        };
};
// function pointer
template <class R, class... Args>
struct function_traits<R (*)(Args...)> : public function_traits<R(Args...)> {};

template <class R, class... Args>
struct function_traits<R(Args...)> {
  using return_type = R;

  static constexpr std::size_t arity = sizeof...(Args);

  template <std::size_t N>
  struct argument {
    static_assert(N < arity, "error: invalid parameter index.");
    using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
  };
};

// member function pointer
template<class C, class R, class... Args>
struct function_traits<R(C::*)(Args...)> : public function_traits<R(C&,Args...)>
{};

// const member function pointer
template<class C, class R, class... Args>
struct function_traits<R(C::*)(Args...) const> : public function_traits<R(C&,Args...)>
{};

// member object pointer
template<class C, class R>
struct function_traits<R(C::*)> : public function_traits<R(C&)>
{};

template<class F>
struct function_traits<F&> : public function_traits<F>
{};

template<class F>
struct function_traits<F&&> : public function_traits<F>
{};

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
struct FunctionArgumentCount{
    static constexpr std::size_t value = function_traits<Func>::arity;
};
//
//template <typename ReturnType, typename... Args>
//struct FunctionArgumentCount<ReturnType(Args...)> {
//  static constexpr std::size_t value = sizeof...(Args);
//};
//
//// Specialization for function pointers
//template <typename ReturnType, typename... Args>
//struct FunctionArgumentCount<ReturnType (*)(Args...)> {
//  static constexpr std::size_t value = sizeof...(Args);
//};
}  // namespace

template <typename HumanModelType, typename MosquitoModelType>
struct model_simulation_fn {
  // Do we want to forward the inputs?
  model_simulation_fn(HumanModelType&& human, MosquitoModelType&& mosquito)
      : human_model_fn(std::forward<HumanModelType>(human)),
        mosquito_model_fn(std::forward<MosquitoModelType>(mosquito)){};

  template <typename Eir, typename... ModelArgs>
  auto operator()(const RealType t0, const RealType t1, const RealType dt,
                  Eir&& initial_eir, ModelArgs&&... model_args) {
    // Split the arguments into the human and mosquito model.
    constexpr auto N_human_args =
        FunctionArgumentCount<HumanModelType>::value - 3;
    auto [human_args, mosquito_args] =
        splitHumanMosquitoArguments<N_human_args>{}(
            std::forward<ModelArgs>(model_args)...);

    // Initial condition does not have to satisfy the model equations.
    auto t = t0;
    auto eir = initial_eir;
    auto foi_mosquitoes = 0.0;
    while (t < t1) {
      // Calculate the mosquito to human interaction.
      auto human_output = std::apply(
          [&](auto&&... args) {
            return human_model_fn(t, dt, eir,
                                  std::forward<decltype(args)>(args)...);
          },
          human_args);
      foi_mosquitoes = human_output.second;
      // Calculate the human mosquito interaction.
      auto mosquito_output = std::apply(
          [&](auto&&... args) {
            return mosquito_model_fn(t, dt, foi_mosquitoes,
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
