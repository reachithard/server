#include <gtest/gtest.h>
#include <iostream>

#include "Random.h"

using namespace Ser;
using namespace std;

TEST(Random, rand)
{
    for (int i = 0; i < 6; i++)
    {
        cout << Rand() << endl;
    }

    for (int i = 0; i < 6; i++) {
        cout << RandRange<int>(10, 20) << endl;
    }

    for (int i = 0; i < 6; i++)
    {
        cout << RandDoubleRange(10.1, 20.1) << endl;
    }

    for (int i = 0; i < 6; i++)
    {
        cout << RandPercent(0.5) << endl;
    }

    for (int i = 0; i < 6; i++)
    {
        cout << RandKey() << endl;
    }
}