
#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <random>
#include <cassert>

namespace fuzzy_coco {

using namespace std;
class RandomGenerator
{
public:
  RandomGenerator(int seed = random_device{}()) : _rng(seed) {}

  int random() { return static_cast<int>(_rng()); }

  int random(int min, int max) {
    // uniform_int_distribution<> distrib(min, max);
    // return distrib(_rng);

    auto x = _rng();
    double norm = double(x) / double(_rng.max() + 1.0);
    return min + int(norm * double(max - min + 1));
  }

  // N.B: append. no reserve
  void random(int min, int max, int nb, vector<int>& stack) {
    // uniform_int_distribution<> distrib(min, max);
    for (int i = 0 ; i < nb; i++)
      stack.push_back(random(min, max));
  }

  template <typename T>
  const T& random(const vector<T>& v) {
    int nb = v.size();
    assert(nb > 0);
    return v[random(0, nb - 1)];
  }

  double randomReal(double min, double max) {
    // uniform_real_distribution<double> distrib(min, max);
    // return distrib(_rng);
    auto x = _rng();
    double norm = double(x) / double(_rng.max() + 1.0);
    return min + (max - min) * norm;
  }

  // N.B: append. no reserve
  void randomReal(double min, double max, int nb, vector<double>& stack) {
    // uniform_real_distribution<double> distrib(min, max);
    for (int i = 0 ; i < nb; i++)
      // stack.push_back(distrib(_rng));
      stack.push_back(randomReal(min, max));
  }

private:
  mt19937 _rng;
};

}
#endif // RANDOM_GENERATOR_H
