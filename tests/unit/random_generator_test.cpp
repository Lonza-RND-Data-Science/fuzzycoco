#include <gtest/gtest.h>
#include <algorithm>
#include "random_generator.h"

using namespace fuzzy_coco;

template<typename T>
ostream& operator<<(ostream& out, const vector<T>& v) {
  out << "{";
  const int nb = v.size() - 1;
  for (int i = 0; i < nb; ++i) {
      out << v[i] << ", "; 
  }
  if (nb >= 0) out << v[nb];
  out << "}";
  return out;
}

TEST(RandomGenerator, random) {
  RandomGenerator rng(123);

  EXPECT_EQ(rng.random(0, 0), 0);
  EXPECT_EQ(rng.random(10, 10), 10);
  EXPECT_EQ(rng.random(-2, -2), -2);

  vector<int> r1;
  rng.random(1, 10, 1000, r1);
  EXPECT_EQ(r1.size(), 1000);
  EXPECT_EQ(*min_element(r1.begin(), r1.end()), 1);
  EXPECT_EQ(*max_element(r1.begin(), r1.end()), 10);
}

TEST(RandomGenerator, randomReal) {
  RandomGenerator rng(456);

  EXPECT_DOUBLE_EQ(rng.randomReal(0, 0), 0);
  EXPECT_EQ(rng.randomReal(0.21, 0.21), 0.21);
  EXPECT_EQ(rng.randomReal(-2.01, -2.01), -2.01);

  vector<double> r1;
  rng.randomReal(-1, 2, 1000, r1);
  EXPECT_EQ(r1.size(), 1000);
  EXPECT_TRUE(*min_element(r1.begin(), r1.end()) >= -1);
  EXPECT_TRUE(*max_element(r1.begin(), r1.end()) <= 2);
}

TEST(RandomGenerator, reproducibility) {
  RandomGenerator rng1(456);
  RandomGenerator rng2(456);
  RandomGenerator rng3(456);

  vector<int> ints1, ints2;
  rng1.random(-1, 3, 100, ints1);
  rng2.random(-1, 3, 100, ints2);
  EXPECT_EQ(ints1, ints2);

  bool all_equal = true;
  for (int i = 0; i < 100; i++)
    if (rng3.random(-1, 3) != ints1[i]) {
      all_equal = false;
      break;
    }
  EXPECT_TRUE(all_equal);  

  vector<double> reals1, reals2;
  rng1.randomReal(-1000.1, -200.04, 100, reals1);
  rng2.randomReal(-1000.1, -200.04, 100, reals2);
  EXPECT_EQ(reals1, reals2);
}

TEST(RandomGenerator, seed) {
  RandomGenerator rng1(123);
  RandomGenerator rng2(456);

  vector<int> ints1, ints2;
  rng1.random(-1, 3, 100, ints1);
  rng2.random(-1, 3, 100, ints2);
  EXPECT_NE(ints1, ints2);
}

TEST(RandomGenerator, vector) {
  RandomGenerator rng1(123);
  RandomGenerator rng2(123);

  int nb = 10;
  vector<double> v(nb);
  for (int i = 0; i < nb; ++i) {
    v[i] = i / 11.2;
  }
  EXPECT_DOUBLE_EQ(rng1.random(v), v[rng2.random(0, nb - 1)]);

}

TEST(mt19937, portability) {
    std::mt19937 rng(666);
    uint_fast32_t expected[10] = {3008354540,440739714,3625754029,907667358,2905606974,553951302,3126126537,3222645150,4086480804,1442973117};

    for (int i = 0; i < 10; i++) {
      EXPECT_EQ(rng(), expected[i]);
    }
}