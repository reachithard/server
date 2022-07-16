#ifndef _LOCKFREERINGBUFFER_H_
#define _LOCKFREERINGBUFFER_H_

#include <stdint.h>
#include <tuple>
#include <optional>
#include <atomic>

#include "NoCopyable.h"
#include "Param.h"
#include "Perf.h"
#include "Barrier.h"

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
    SyncType sync = SyncType::SER_SINGLE;
} HeadTailT;

class LockFreeRingBuffer : public NoCopyable
{
  public:
    LockFreeRingBuffer(uint32_t cnt, uint32_t eltsize, void *buffer)
        : eltsize(eltsize),
          ring(buffer),
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

    uint32_t EnqueBulk(const void *buffer,
                       uint32_t cnt,
                       uint32_t &free,
                       HeadTailT &cons)
    {
        return DoEnqueBulk(buffer, cnt, free, cons);
    }

    uint32_t DequeBulk(void *buffer,
                       uint32_t cnt,
                       uint32_t &outCnt,
                       uint32_t &avaliable,
                       HeadTailT &cons)
    {
        return DoDequeBulk(buffer, cnt, avaliable, cons);
    }

  private:
    void UpdateTail(HeadTailT &tail,
                    uint32_t oldPos,
                    uint32_t newPos,
                    bool isSingle,
                    bool enqueue)
    {
        if (enqueue)
        {
            ser_smp_wmb();
        }
        else
        {
            ser_smp_rmb();
        }

        if (!isSingle)
        {
            while (tail.tail.load(std::memory_order_relaxed) != oldPos)
            {
                ;  // 空转
            }
        }
        tail.tail = newPos;
    }

    /**************************************************************** enqueue
     * begin ****************************************/
    uint32_t MoveProdHead(uint32_t cnt,
                          uint32_t &free,
                          HeadTailT &cons,
                          bool isSingle,
                          uint32_t &prodHead,
                          uint32_t &prodNext)
    {
        const uint32_t cap = capacity;
        uint32_t max = cnt;
        bool success = false;

        do
        {
            cnt = max;
            prodHead = prod.head;

            // 内存屏障
            ser_smp_rmb();

            free = (cap + cons.tail - prodHead);

            // 固定值判断
            prodNext = prodHead + cnt;
            if (isSingle)
            {
                prod.head = prodNext;
                success = true;
            }
            else
            {
                success = prod.head.compare_exchange_weak(prodHead, prodNext);
            }
        } while (unlikely(success == false));
        return cnt;
    }

    void EnqueueElems(const void *buffer, uint32_t prodHead, uint32_t cnt)
    {
        // 进行数据的拷贝
        uint32_t idx = prodHead % mask;  // prodHead会一直增长 直至回绕
        char *buff = (char *)buffer;
        char *r = (char *)ring;
        if (likely(idx + cnt <= size))
        {
            memcpy(r + (idx * eltsize), buff, eltsize * cnt);
        }
        else
        {
            // 这里需要拷贝两次 一次尾部 一次头部
            memcpy(r + (idx * eltsize), buff, eltsize * (size - idx - 1));
            memcpy(r,
                   buff + eltsize * (size - idx - 1),
                   eltsize * (cnt - (size - idx - 1)));
        }
    }

    uint32_t DoEnqueueElem(const void *buffer,
                           uint32_t cnt,
                           uint32_t &free,
                           HeadTailT &cons,
                           bool isSingle)
    {
        // 分为两阶段提交 1。移动prod头 2.入队 3.更新尾部
        uint32_t prodHead = 0;
        uint32_t prodNext = 0;
        uint32_t freeEntries = 0;
        cnt =
            MoveProdHead(cnt, freeEntries, cons, isSingle, prodHead, prodNext);

        if (cnt == 0)
        {
            goto end;
        }

        // 入队
        EnqueueElems(buffer, prodHead, cnt);

        // 更新尾部
        UpdateTail(prod, prodHead, prodNext, isSingle, true);

    end:
        free = freeEntries - cnt;
        return cnt;
    }

    uint32_t DoSpEnqueBulk(const void *buffer,
                           uint32_t cnt,
                           uint32_t &free,
                           HeadTailT &cons)
    {
        return DoEnqueueElem(buffer, cnt, free, cons, true);
    }

    uint32_t DoMpEnqueBulk(const void *buffer,
                           uint32_t cnt,
                           uint32_t &free,
                           HeadTailT &cons)
    {
        return DoEnqueueElem(buffer, cnt, free, cons, false);
    }

    uint32_t DoEnqueBulk(const void *buffer,
                         uint32_t cnt,
                         uint32_t &free,
                         HeadTailT &cons)
    {
        uint32_t ret = 0;
        switch (prod.sync)
        {
            case SyncType::SER_SINGLE:
                /* code */
                ret = DoSpEnqueBulk(buffer, cnt, free, cons);
                break;
            default:
                ret = DoMpEnqueBulk(buffer, cnt, free, cons);
                break;
        }
        return ret;
    }

    /**************************************************************** enqueue
     * end ****************************************/

    /**************************************************************** dequeue
     * begin ****************************************/
    uint32_t MoveConsHead(uint32_t cnt,
                          uint32_t &entries,
                          HeadTailT &cons,
                          bool isSingle,
                          uint32_t &consHead,
                          uint32_t &consNext)
    {
        uint32_t max = cnt;
        bool success = false;

        do
        {
            cnt = max;
            consHead = cons.head;

            // 内存屏障
            ser_smp_rmb();

            entries = (prod.tail - consHead);

            // 固定值判断
            if (cnt > entries) {
              cnt = entries;
            }

            if (unlikely(cnt == 0)) {
                return 0;
            }

            consNext = consHead + cnt;
            if (isSingle)
            {
                cons.head = consNext;
                ser_smp_rmb();
                success = true;
            }
            else
            {
                success = cons.head.compare_exchange_weak(consHead, consNext);
            }
        } while (unlikely(success == false));
        return cnt;
    }

    void DequeueElems(void *buffer, uint32_t consHead, uint32_t cnt)
    {
        // 进行数据的拷贝
        uint32_t idx = consHead % mask;  // 等回绕
        char *buff = (char *)buffer;
        char *r = (char *)ring;
        if (likely(idx + cnt <= size))
        {
            memcpy(buff, r + (idx * eltsize), eltsize * cnt);
        }
        else
        {
            memcpy(buff, r + (idx * eltsize), eltsize * (size - idx - 1));
            memcpy(buff + eltsize * (size - idx - 1),
                   r,
                   eltsize * (cnt - (size - idx - 1)));
        }
    }

    uint32_t DoDequeueElem(void *buffer,
                           uint32_t cnt,
                           uint32_t &avaliable,
                           HeadTailT &cons,
                           bool isSingle)
    {
        // 分为两阶段提交
        uint32_t consHead = 0;
        uint32_t consNext = 0;
        uint32_t entries = 0;
        cnt = MoveConsHead(cnt, entries, cons, isSingle, consHead, consNext);

        if (cnt == 0)
        {
            goto end;
        }

        // 出队
        DequeueElems(buffer, consHead, cnt);

        // 更新尾部
        UpdateTail(cons, consHead, consNext, isSingle, false);

    end:
        avaliable = entries - cnt;
        return cnt;
    }

    uint32_t DoSpDequeBulk(void *buffer,
                           uint32_t cnt,
                           uint32_t &avaliable,
                           HeadTailT &cons)
    {
        return DoDequeueElem(buffer, cnt, avaliable, cons, true);
    }

    uint32_t DoMpDequeBulk(void *buffer,
                           uint32_t cnt,
                           uint32_t &avaliable,
                           HeadTailT &cons)
    {
        return DoDequeueElem(buffer, cnt, avaliable, cons, false);
    }

    uint32_t DoDequeBulk(void *buffer,
                         uint32_t cnt,
                         uint32_t &avaliable,
                         HeadTailT &cons)
    {
        uint32_t ret = 0;
        switch (cons.sync)
        {
            case SyncType::SER_SINGLE:
                /* code */
                ret = DoSpDequeBulk(buffer, cnt, avaliable, cons);
                break;
            default:
                ret = DoMpDequeBulk(buffer, cnt, avaliable, cons);
                break;
        }
        return ret;
    }

    /**************************************************************** dequeue
     * end ****************************************/

  private:
    uint32_t eltsize;
    void *ring;
    uint32_t size;
    uint32_t mask;
    uint32_t capacity;
    char pad1;
    HeadTailT prod;
};
}  // namespace Ser

#endif  // _LOCKFREERINGBUFFER_H_