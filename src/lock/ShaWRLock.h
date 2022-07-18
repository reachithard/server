#ifndef _SHAWRLOCK_H_
#define _SHAWRLOCK_H_

#include <atomic>
#include <cstdint>

#include "NoCopyable.h"

namespace Ser
{
// 因为在共享内存里面锁都是无效的 所以自己实现
class ShaWRlock
{
  public:
    void RdLock()
    {
        int32_t tmp = 0;
        bool success = false;

        while (!success)
        {
            tmp = lk.load(std::memory_order_relaxed);
            if (tmp < 0)
            {
                continue;
            }

            success = lk.compare_exchange_weak(tmp,
                                               tmp + 1,
                                               std::memory_order_acquire,
                                               std::memory_order_relaxed);
        }
    }

    void RdUnLock()
    {
        lk.fetch_sub(1, std::memory_order_release);
    }

    bool TryRdLock()
    {
        int32_t tmp = 0;
        bool success = false;

        while (!success)
        {
            tmp = lk.load(std::memory_order_relaxed);
            if (tmp < 0)
            {
                return false;
            }

            success = lk.compare_exchange_weak(tmp,
                                               tmp + 1,
                                               std::memory_order_acquire,
                                               std::memory_order_relaxed);
        }

        return true;
    }

    void WrLock()
    {
        int32_t tmp = 0;
        bool success = false;

        while (!success)
        {
            tmp = lk.load(std::memory_order_relaxed);
            if (tmp != 0)
            {
                continue;
            }

            success = lk.compare_exchange_weak(tmp,
                                               -1,
                                               std::memory_order_acquire,
                                               std::memory_order_relaxed);
        }
    }

    void WrUnLock()
    {
        lk.store(0, std::memory_order_release);
    }

    bool TryWrLock()
    {
        int32_t tmp = lk.load(std::memory_order_relaxed);
        return tmp != 0 || lk.compare_exchange_weak(tmp,
                                                    -1,
                                                    std::memory_order_acquire,
                                                    std::memory_order_release);
    }

  private:
    std::atomic<int32_t> lk{0};
};
}  // namespace Ser

#endif  // _SHAWRLOCK_H_