#include <gtest/gtest.h>
#include <iostream>
#include <chrono>
#include <memory>
#include <functional>
#include <fstream>

#include "SpinLock.h"
#include "Barrier.h"

using namespace Ser;
using namespace std;

TEST(SpinLock, lock)
{
    int cnt = 0;
    SpinLock lock;
    std::function<void()> th = [&cnt, &lock]() {
        lock.lock();
        for (int i = 0; i < 1000; i++)
        {
            cnt++;
        }
        lock.unlock();
    };

    std::thread th1(th);
    std::thread th2(th);

    th1.join();
    th2.join();

    ASSERT_EQ(cnt, 2000);
}

TEST(SpinLock, volatile)
{
    volatile int cnt = 0;
    std::function<void()> th = [&cnt]() {
        for (int i = 0; i < 1000; i++)
        {
            cnt++;
        }
    };

    std::thread th1(th);
    std::thread th2(th);

    th1.join();
    th2.join();

    ASSERT_EQ(cnt, 2000); // 可见内存屏障也是安全的
}

TEST(SpinLock, Barrier)
{
    int cnt = 0;
    std::function<void()> th = [&cnt]() {
        for (int i = 0; i < 1000; i++)
        {
            ser_smp_wmb(); // 这一个就是加入内存屏障而已
            cnt++;
        }
    };

    std::thread th1(th);
    std::thread th2(th);

    th1.join();
    th2.join();

    ASSERT_EQ(cnt, 2000);  // 可见内存屏障也是安全的
}