#ifndef PLASX_DETAILS_FUNCTION_ARGS_HPP
#define PLASX_DETAILS_FUNCTION_ARGS_HPP
#include <tuple>
namespace plasx {
// functor
template <class F>
struct FunctionArgumentCounter {
  static constexpr std::size_t value =
      FunctionArgumentCounter<decltype(&F::operator())>::value - 1;
};
// function pointer
template <class R, class... Args>
struct FunctionArgumentCounter<R (*)(Args...)>
    : public FunctionArgumentCounter<R(Args...)> {};

template <class R, class... Args>
struct FunctionArgumentCounter<R(Args...)> {
  static constexpr std::size_t value = sizeof...(Args);
};

// member function pointer
template <class C, class R, class... Args>
struct FunctionArgumentCounter<R (C::*)(Args...)>
    : public FunctionArgumentCounter<R(C&, Args...)> {};

// const member function pointer
template <class C, class R, class... Args>
struct FunctionArgumentCounter<R (C::*)(Args...) const>
    : public FunctionArgumentCounter<R(C&, Args...)> {};

// member object pointer
template <class C, class R>
struct FunctionArgumentCounter<R(C::*)>
    : public FunctionArgumentCounter<R(C&)> {};

template <class F>
struct FunctionArgumentCounter<F&> : public FunctionArgumentCounter<F> {};

template <class F>
struct FunctionArgumentCounter<F&&> : public FunctionArgumentCounter<F> {};

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
}  // namespace plasx
#endif
