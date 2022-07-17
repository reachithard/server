#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <atomic>
#include <thread>

#include "NoCopyable.h"

namespace Ser {
class SpinLock : public NoCopyable
{
  public:
    void lock() noexcept {
        while (true)
        {
            if (!lk.exchange(true, std::memory_order_acquire))
            {
                return;
            }
            uint32_t counter = 0;
            while (lk.load(std::memory_order_relaxed))
            {
                if (++counter > 16) {
                    std::this_thread::yield();
                }
            }
        }
        
    }

    void unlock() noexcept {

    }


  private:
    std::atomic<bool> lk{false};
};
}

#endif  // _SPINLOCK_H_