#include <gtest/gtest.h>
#include <iostream>

#include "Runtime.h"

using namespace Ser;
using namespace std;

TEST(Runtime, Init)
{
    Singleton<Runtime>::Instance().Init();
}