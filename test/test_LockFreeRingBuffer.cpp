#include <gtest/gtest.h>

#include <iostream>
#include <type_traits>
#include <memory>

#include "LockFreeRingBuffer.h"
#include "Param.h"

using namespace Ser::Detail;

using namespace Ser;
using namespace std;

typedef struct MallocDataS
{
    uint64_t seq;
    char buffer[1024];
    uint32_t t;
    float p;
    double d;

    void Dump()
    {
        cout << "seq:" << seq << " buffer:" << std::string(buffer) << " t:" << t
             << " p:" << p << " d:" << d << endl;
    }
} MallocDataT;

TEST(LockFreeRingBuffer, mallocAllocator)
{
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
    for (int idx = 0; idx < size; idx++)
    {
        for (int tt = 0; tt < wrSize; tt++)
        {
            writer[tt].seq = seq;
            seq++;
        }
        ret = ringBuffer->EnqueBulk(writer, wrSize, free, cons);
        cout << "ret:" << ret << " free:" << free << endl;
    }

    constexpr uint32_t rdSize = 1;
    MallocDataT *reader = (MallocDataT *)malloc(sizeof(MallocDataT) * rdSize);
    uint32_t outLen = 0;
    uint32_t avaliable = 0;

    for (int idx = 0; idx < size; idx++)
    {
        ret = ringBuffer->DequeBulk(reader, rdSize, outLen, avaliable, cons);
        cout << "ret:" << ret << " outLen:" << outLen
             << " avaliable:" << avaliable << endl;

        for (int tmp = 0; tmp < rdSize; tmp++)
        {
            reader[tmp].Dump();
        }
    }
}