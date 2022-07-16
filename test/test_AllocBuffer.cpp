#include <gtest/gtest.h>

#include <iostream>

#include "AllocBuffer.h"

using namespace Ser::Detail;
using namespace std;

TEST(AllocBuffer, RuntimeStorage)
{
    RuntimeStorage<int> p(5);
    cout << p.getCnt() << endl;
}