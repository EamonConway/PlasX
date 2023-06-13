#include <chrono>
#include <iostream>

#include "PlasX/Falciparum/griffin.hpp"
#include "PlasX/simulation.hpp"
#include "PlasX/udl.hpp"

using namespace plasx;
namespace pfg = falciparum::griffin;

int main() {
  // Create parameters.
  pfg::Parameters params;
  const int N = 1000000;
  // Create individuals.
  std::vector<Individual<pfg::PFalc>> population;
  for (auto i = 0; i < N; ++i) {
    population.emplace_back(10.0, pfg::Status::S, 0.0, 0.0, 0.0);
  }

  // Run a simulation - as this function is templated, it should be possible to
  // add arbitrary numbers of inputs. This will be helpful for establishing
  // any interventions.
  std::vector<size_t> S, A, U, D, T, P, Dead;
  for (int i = 0; i < population.size(); ++i) {
    S.emplace_back(i);
  }
  A.reserve(N);
  U.reserve(N);
  D.reserve(N);
  T.reserve(N);
  P.reserve(N);


  auto start = std::chrono::steady_clock::now();
  [[maybe_unused]] auto t2 =
      plasx::simulation(0.0_yrs, 1.0_yrs, 1.0_days, pfg::one_step_no_switch,
                        population, params, 1.0,S, A, U, D, T, P, Dead);
  auto end = std::chrono::steady_clock::now();
  auto elapsed_seconds = end - start;
  std::cout << "elapsed time with no switches: " << elapsed_seconds.count() * 1e-6 << "\n";
  std::cout << "pop size " << population.size() << std::endl;

  return EXIT_SUCCESS;
}