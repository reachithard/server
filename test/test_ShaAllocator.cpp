#include <gtest/gtest.h>
#include <iostream>
#include <chrono>
#include <memory>
#include <functional>
#include <fstream>

#include "ShaAllocator.h"
#include "LockFreeRingBuffer.h"
#include "Random.h"

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

    // 重载 << 运算符
    friend ostream &operator<<(ostream &out, struct MallocDataS &a)
    {
        out << "seq:" << a.seq;
        out << std::endl;
        // out << " buffer:" << std::string(a.buffer);

        // out << " t: ";
        // for (int idx = 0; idx < MALLOC_DATA_SIZE; idx++)
        // {
        //     out << a.t[idx] << ",";
        // }

        // out << " p: ";
        // for (int idx = 0; idx < MALLOC_DATA_SIZE; idx++)
        // {
        //     out << a.p[idx] << ",";
        // }

        // out << " d: ";
        // for (int idx = 0; idx < MALLOC_DATA_SIZE; idx++)
        // {
        //     out << a.d[idx] << ",";
        // }
        // out << endl;
        return out;
    }

} MallocDataT;



TEST(ShaAllocator, Anon)
{
    ShaAllocator allocator;

    uint32_t size = 600000;
    int loopTimes = 1000000;
    size_t all = sizeof(MallocDataT) * loopTimes;
    cout << "size:" << (all >> 20) << "mb" << endl;

    char *buffer = allocator.allocate(sizeof(MallocDataT) * size);
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
        std::chrono::duration<double>(end.time_since_epoch() -
                                      start.time_since_epoch())
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
    elapsed_seconds = std::chrono::duration<double>(end.time_since_epoch() -
                                                    start.time_since_epoch())
                          .count();

    std::cout << "elapsed time: " << elapsed_seconds << "us\n";

    allocator.deallocate(buffer, sizeof(MallocDataT) * size);
}

TEST(ShaAllocator, AnonRandomAndCheck)
{
    ShaAllocator allocator;

    uint32_t size = 300000;
    int loopTimes = 400000;
    size_t all = sizeof(MallocDataT) * loopTimes;
    cout << "size:" << (all >> 20) << "mb" << endl;

    char *buffer = allocator.allocate(sizeof(MallocDataT) * size);
    unique_ptr<LockFreeRingBuffer> ringBuffer =
        make_unique<LockFreeRingBuffer>(size, sizeof(MallocDataS), buffer);

    HeadTailT cons;

    vector<shared_ptr<MallocDataT>> writerReport;
    writerReport.reserve(loopTimes);

    // 开启线程测试一下 然后比对一下输出文件
    function<void()> writerFunc = [loopTimes, &writerReport, &ringBuffer, &cons]() {
        uint64_t seq = 0;
        uint32_t free = 0;
        uint32_t ret = 0;
        constexpr uint32_t wrSize = 1;
        MallocDataT writer[wrSize] = {0};

        auto start = std::chrono::steady_clock::now();
        for (int idx = 0; idx < loopTimes; idx++)
        {
            for (int tt = 0; tt < wrSize; tt++)
            {
                writer[tt].seq = seq;
                seq++;

                strncpy(writer[tt].buffer, (const char*)RandKey(), 149);
                writer[tt].buffer[149] = '\0';

                double tmpd =
                    RandDoubleRange(1.01, 2.01);

                float tmpp = RandDoubleRange<float>(1.01, 2.01);

                uint32_t tmpt = RandRange<int>(5, 10);

                    for (int pp = 0; pp < MALLOC_DATA_SIZE; pp++)
                {
                    writer[tt].d[pp] = tmpd;
                    writer[tt].p[pp] = tmpp;
                    writer[tt].t[pp] = tmpt;
                }

                // 这里先不做数据检验了
                // shared_ptr<MallocDataT> ptr =
                //     make_shared<MallocDataT>(writer[tt]);
                // writerReport.push_back(ptr);
            }
            ret = ringBuffer->EnqueBulk(writer, wrSize, free, cons);
            // cout << "ret:" << ret << " free:" << free << endl;
        }

        auto end = std::chrono::steady_clock::now();
        auto elapsed_seconds =
            std::chrono::duration<double>(end.time_since_epoch() -
                                          start.time_since_epoch())
                .count();

        ringBuffer->Dump(cons);
        std::cout << "writer elapsed time: " << elapsed_seconds
                             << "s\n";
    };

    vector<shared_ptr<MallocDataT>> readerReport;
    readerReport.reserve(loopTimes);

    function<void()> readerFunc = [loopTimes, &ringBuffer, &cons, &readerReport]() {
        constexpr uint32_t rdSize = 1;
        MallocDataT *reader =
            (MallocDataT *)malloc(sizeof(MallocDataT) * rdSize);
        uint32_t outLen = 0;
        uint32_t avaliable = 0;
        uint32_t ret = 0;

        auto start = std::chrono::steady_clock::now();

        while (true) {
            ret =
                ringBuffer->DequeBulk(reader, rdSize, outLen, avaliable, cons);
            // cout << "ret:" << ret << " outLen:" << outLen
            //      << " avaliable:" << avaliable << endl;
            if (readerReport.size() >= loopTimes)
            {
                break;
            }

            if (outLen == 0) {
                // ringBuffer->Dump(cons);
                continue;
            }

            for (int tmp = 0; tmp < rdSize; tmp++)
            {
                shared_ptr<MallocDataT> ptr =
                    make_shared<MallocDataT>(reader[tmp]);
                readerReport.push_back(ptr);
            }
        }
        auto end = std::chrono::steady_clock::now();
        auto elapsed_seconds = std::chrono::duration<double>(
                              end.time_since_epoch() - start.time_since_epoch())
                              .count();

        std::cout << "reader elapsed time: " << elapsed_seconds << "us\n";
    };

    std::thread wrThread(writerFunc);
    std::thread rdThread(readerFunc);

    wrThread.join();
    rdThread.join();

    std::string wrfilename = "ShaAllocator_AnonRandomAndCheck_Writer.log";
    std::ofstream wrStream(wrfilename, std::ios::app | std::ios::out);

    for (int t = 0; t < writerReport.size(); t++) {
        wrStream << *writerReport[t].get();
    }

    std::string rdfilename = "ShaAllocator_AnonRandomAndCheck_Reader.log";
    std::ofstream rdStream(rdfilename, std::ios::app | std::ios::out);

    for (int t = 0; t < readerReport.size(); t++)
    {
        rdStream << *readerReport[t].get();
    }

    allocator.deallocate(buffer, sizeof(MallocDataT) * size);
}