#ifndef _LOCKFREERINGBUFFER_H_
#define _LOCKFREERINGBUFFER_H_

#include <stdint.h>
#include <tuple>
#include <optional>
#include <atomic>

#include "NoCopyable.h"
#include "Param.h"

namespace Ser
{
// 之后把allocator抽象出来
enum class SyncType
{
    SER_MUTIL,
    SER_SINGLE,
};

typedef struct HeadTailS
{
    std::atomic<uint32_t> head{0};
    std::atomic<uint32_t> tail{0};
    SyncType sync = SyncType::SER_MUTIL;
} HeadTailT;

class LockFreeRingBuffer : public NoCopyable
{
  public:
    LockFreeRingBuffer(uint32_t cnt, uint32_t eltsize, void *buffer)
        : eltsize(eltsize),
          buffer(buffer),
          size(cnt + 1),
          mask(cnt),
          capacity(cnt)
    {
    }
    ~LockFreeRingBuffer()
    {
    }

    uint32_t Cnt(const HeadTailT &cons)
    {
        uint32_t prodTail = prod.tail;
        uint32_t consTail = cons.tail;
        uint32_t cnt = (prodTail - consTail) % mask;
        return cnt;
    }

    uint32_t FreeCnt(const HeadTailT &cons)
    {
        return capacity - Cnt(cons);
    }

    bool Full(const HeadTailT &cons)
    {
        return FreeCnt(cons) == 0;
    }

    bool Empty(const HeadTailT &cons)
    {
        uint32_t prodTail = prod.tail;
        uint32_t consTail = cons.tail;
        return consTail == prodTail;
    }

    uint32_t EnqueBulk(void *buffer,
                       uint32_t size,
                       uint32_t &free,
                       HeadTailT &cons)
    {
        // 分为两阶段提交 1。移动prod头 2.入队 3.出队

    }

    uint32_t DequeBulk(void *buffer,
                       uint32_t cnt,
                       uint32_t &outCnt,
                       HeadTailT &cons)
    {
    }



  private:
    uint32_t DoEnqueBulk(void *buffer,
                         uint32_t size,
                         uint32_t &free,
                         HeadTailT &cons)
    {
      
    }

  private:
    uint32_t eltsize;
    void *buffer;
    uint32_t size;
    uint32_t mask;
    uint32_t capacity;
    char pad1;
    HeadTailT prod;
};
}  // namespace Ser

#endif  // _LOCKFREERINGBUFFER_H_