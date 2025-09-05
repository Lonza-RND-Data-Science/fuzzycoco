
#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <random>
#include <cassert>
#include <cstring>

namespace fuzzy_coco {

using namespace std;
class RandomGenerator
{
public:
  RandomGenerator(int seed = random_device{}()) : _rng(seed) {}

  uint32_t random() { return _rng(); }

  int random(int min, int max) {
    // uniform_int_distribution<> distrib(min, max);
    // return distrib(_rng);
    return scale_int(random(), min, max);
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
    return scale_int_to_double(random(), min, max);
    // auto x = _rng();
    // double norm = double(x) / double(_rng.max() + 1.0);
    // return min + (max - min) * norm;
  }

  // N.B: append. no reserve
  void randomReal(double min, double max, int nb, vector<double>& stack) {
    // uniform_real_distribution<double> distrib(min, max);
    for (int i = 0 ; i < nb; i++)
      // stack.push_back(distrib(_rng));
      stack.push_back(randomReal(min, max));
  }

  static int scale_int(uint32_t x, int min, int max) {
    double norm = double(x) / double(mt19937::max() + 1.0);
    return min + int(norm * double(max - min + 1));
  }

  static double scale_int_to_double(uint32_t x, double min, double max) {
    double norm = double(x) / double(mt19937::max() + 1.0);
    return min + (max - min) * norm;
  }

  static double scale_int_to_double2(uint32_t x, double min, double max) {
    double norm = std::ldexp(x, -32);
    return min + (max - min) * norm;
  }

  static double scale_int_to_double3(uint32_t x, double min, double max) {
    // Convert to [0,1) by integer-to-double scaling
    uint64_t numerator = x;
    double norm = static_cast<double>(numerator) / 4294967296.0; // 2^32, exact
    // Force rounding now
    uint64_t bits;
    std::memcpy(&bits, &norm, sizeof(norm));
    std::memcpy(&norm, &bits, sizeof(norm));  // normalize rounding
    return min + (max - min) * norm;
  }

  static double scale_int_to_double4(uint32_t x, double min, double max) {
    // Exact fraction in [0,1)
    double norm = std::ldexp(static_cast<double>(x), -32);
    // Force canonical rounding to double
    uint64_t bits;
    std::memcpy(&bits, &norm, sizeof(norm));
    std::memcpy(&norm, &bits, sizeof(norm));
    return min + (max - min) * norm;
  }

private:
  mt19937 _rng;
};

}
#endif // RANDOM_GENERATOR_H
