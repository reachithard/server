#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <atomic>
#include <thread>

#include "NoCopyable.h"

namespace Ser {
class SpinLock : public NoCopyable
{
  public:
    inline void pause() {

    }

    void lock()
    {
        for (int spin_count = 0; !try_lock(); ++spin_count)
        {
            if (spin_count < 16) {
                pause();
            }
            else
            {
                std::this_thread::yield();
                spin_count = 0;
            }
        }
    }
    bool try_lock()
    {
        return !lk.load(std::memory_order_relaxed) &&
               !lk.exchange(true, std::memory_order_acquire);
    }
    void unlock()
    {
        lk.store(false, std::memory_order_release);
    }

  private:
    std::atomic<bool> lk{false};
};
}

#endif  // _SPINLOCK_H_