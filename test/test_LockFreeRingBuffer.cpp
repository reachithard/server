#include <gtest/gtest.h>

#include <iostream>
#include <type_traits>

#include "LockFreeRingBuffer.h"
#include "Param.h"

using namespace Ser::Detail;

using namespace Ser;
using namespace std;

TEST(Param, Contain) {
  cout << contains<int, short, char, short>::value << endl;
  cout << contains<int, short, char, short, int>::value << endl;

  cout << is_same_v<int, hasArgs<int, int, char, long>::type> << endl;
  cout << hasArgs<int, int, char, long>::value << endl;
}

TEST(LockFreeRingBuffer, compile) {
}