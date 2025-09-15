#ifndef TEST_H
#define TEST_H

#include <gtest/gtest.h>


// utilities for the unit tests
#include <numeric>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

auto sum = [](auto v) { return std::accumulate(v.begin(), v.end(), 0); };

template<typename T, class UnaryPred>
bool all(const vector<T>& v, UnaryPred pred) { 
  return std::all_of(v.cbegin(), v.cend(), pred);
}

template<typename T>
bool all_zero(const vector<T>& v) { return all(v, [](T t) { return t == 0; });
}


// template<typename T>
// ostream& operator<<(ostream& out, const vector<T>& v) {
//   out << "{";
//   const int nb = v.size() - 1;
//   for (int i = 0; i < nb; ++i) {
//       out << v[i] << ", "; 
//   }
//   if (nb >= 0) out << v[nb];
//   out << "}";
//   return out;
// }



#endif