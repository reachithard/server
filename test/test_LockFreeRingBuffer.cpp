#include <gtest/gtest.h>

#include <iostream>
#include <type_traits>
#include <memory>
#include <chrono>
#include <sys/mman.h>

#include "LockFreeRingBuffer.h"
#include "Param.h"

using namespace Ser::Detail;

using namespace Ser;
using namespace std;

// 为630时则刚好为
#define MALLOC_DATA_SIZE 630
typedef struct MallocDataS
{
    uint64_t seq;
    char buffer[150];
    uint32_t t[MALLOC_DATA_SIZE];
    float p[MALLOC_DATA_SIZE];
    double d[MALLOC_DATA_SIZE];

    void Dump()
    {
        cout << "seq:" << seq << " buffer:" << std::string(buffer) << " t:" << t
             << " p:" << p << " d:" << d << endl;
    }
} MallocDataT;

TEST(LockFreeRingBuffer, mallocAllocator)
{
    cout << "size:" << sizeof(MallocDataT) << "byte" << endl;
    uint32_t size = 1024;
    void *buffer = malloc(size * sizeof(MallocDataT));
    unique_ptr<LockFreeRingBuffer> ringBuffer =
        make_unique<LockFreeRingBuffer>(size, sizeof(MallocDataS), buffer);

    constexpr uint32_t wrSize = 1;
    MallocDataT writer[wrSize] = {0};
    uint64_t seq = 0;
    HeadTailT cons;
    uint32_t free = 0;
    uint32_t ret = 0;
    auto start = std::chrono::steady_clock::now();
    int loopsize = 1000000;
    for (int idx = 0; idx < loopsize; idx++)
    {
        for (int tt = 0; tt < wrSize; tt++)
        {
            writer[tt].seq = seq;
            seq++;
        }
        ret = ringBuffer->EnqueBulk(writer, wrSize, free, cons);
        // cout << "ret:" << ret << " free:" << free << endl;
    }

    auto end = std::chrono::steady_clock::now();
    auto elapsed_seconds =
        std::chrono::duration_cast<std::chrono::microseconds>(end.time_since_epoch() - start.time_since_epoch()).count();

    std::cout << "elapsed time: " << elapsed_seconds << "us\n";
    constexpr uint32_t rdSize = 1;
    MallocDataT *reader = (MallocDataT *)malloc(sizeof(MallocDataT) * rdSize);
    uint32_t outLen = 0;
    uint32_t avaliable = 0;

    start = std::chrono::steady_clock::now();
    for (int idx = 0; idx < size; idx++)
    {
        ret = ringBuffer->DequeBulk(reader, rdSize, outLen, avaliable, cons);
        // cout << "ret:" << ret << " outLen:" << outLen
        //      << " avaliable:" << avaliable << endl;

        // for (int tmp = 0; tmp < rdSize; tmp++)
        // {
        //     reader[tmp].Dump();
        // }
    }
    end = std::chrono::steady_clock::now();
    elapsed_seconds = std::chrono::duration_cast<std::chrono::microseconds>(
                          end.time_since_epoch() - start.time_since_epoch())
                          .count();

    std::cout << "elapsed time: " << elapsed_seconds << "us\n";
}

// 该测试用例是共享内存的
TEST(LockFreeRingBuffer, sharedMemSpSc)
{
    cout << "size:" << sizeof(MallocDataT) << "byte" << endl;

    uint32_t size = 300000;
    int loopTimes = 1000000;
    size_t all = sizeof(MallocDataT) * loopTimes;
    cout << "size:" << (all >> 20) << "mb" << endl;

    void *buffer = mmap(NULL,
                        sizeof(MallocDataT) * size,
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANON,
                        -1,
                        0);

    if (buffer == MAP_FAILED)
    {
        printf("map failed! %s\n", strerror(errno));
    }

    unique_ptr<LockFreeRingBuffer> ringBuffer =
        make_unique<LockFreeRingBuffer>(size, sizeof(MallocDataS), buffer);

    constexpr uint32_t wrSize = 1;
    MallocDataT writer[wrSize] = {0};
    uint64_t seq = 0;
    HeadTailT cons;
    uint32_t free = 0;
    uint32_t ret = 0;
    auto start = std::chrono::steady_clock::now();
    for (int idx = 0; idx < loopTimes; idx++)
    {
        for (int tt = 0; tt < wrSize; tt++)
        {
            writer[tt].seq = seq;
            seq++;
        }
        ret = ringBuffer->EnqueBulk(writer, wrSize, free, cons);
        // cout << "ret:" << ret << " free:" << free << endl;
    }

    auto end = std::chrono::steady_clock::now();
    auto elapsed_seconds =
        std::chrono::duration<double>(
            end.time_since_epoch() - start.time_since_epoch())
            .count();

    std::cout << "elapsed time: " << elapsed_seconds << "s\n";
    constexpr uint32_t rdSize = 1;
    MallocDataT *reader = (MallocDataT *)malloc(sizeof(MallocDataT) * rdSize);
    uint32_t outLen = 0;
    uint32_t avaliable = 0;

    start = std::chrono::steady_clock::now();
    for (int idx = 0; idx < loopTimes; idx++)
    {
        ret = ringBuffer->DequeBulk(reader, rdSize, outLen, avaliable, cons);
        // cout << "ret:" << ret << " outLen:" << outLen
        //      << " avaliable:" << avaliable << endl;

        // for (int tmp = 0; tmp < rdSize; tmp++)
        // {
        //     reader[tmp].Dump();
        // }
    }
    end = std::chrono::steady_clock::now();
    elapsed_seconds =
        std::chrono::duration<double>(end.time_since_epoch() -
                                      start.time_since_epoch())
            .count();

    std::cout << "elapsed time: " << elapsed_seconds << "us\n";

    munmap(buffer, sizeof(MallocDataT) * size);
}

TEST(LockFreeRingBuffer, sharedMemIngSpSc)
{

}