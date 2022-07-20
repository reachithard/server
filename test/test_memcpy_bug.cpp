#include <gtest/gtest.h>
#include <iostream>
// 提供mmap函数
#include <sys/mman.h>
#include <fstream>

using namespace std;

// Demonstrate some basic assertions.
typedef struct tps
{
    int h;
    char t;
    double x;
    char q[5400];
} tpt;
TEST(Bug, memcpy)
{
    cout << "sizeof:" << sizeof(tpt) << endl;

    int size = 10000;
    int prot = PROT_READ | PROT_WRITE;

    int flag = MAP_SHARED | MAP_ANON;

    void *buffer =
        mmap(NULL, sizeof(tpt) * size, prot, MAP_SHARED | MAP_ANON, -1, 0);

    if (buffer == MAP_FAILED)
    {
        printf("map failed! %s\n", strerror(errno));
    }

    tpt st = {.h = 100, .t = 'a'};
    int offset = 0;
    for (int i = 0; i < size; i++)
    {
        memcpy((char*)buffer + offset, &st, sizeof(tpt));
        offset += sizeof(tpt);
    }

    offset = 0;
    std::string wrfilename = "ShaAllocator_AnonRandomAndCheck_Writer.log";
    std::ofstream wrStream(wrfilename, std::ios::app | std::ios::out);
    for (int i = 0; i < size; i++)
    {
        tpt tm;
        memcpy(&tm, (char *)buffer + offset, sizeof(tpt));
        offset += sizeof(tpt);
        wrStream << "h:" << tm.h << " t:" << tm.t << " i:" << i << endl;
    }

    munmap(buffer, sizeof(tpt) * size);
}