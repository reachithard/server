#ifndef _MCSLOCK_H_
#define _MCSLOCK_H_

#include <atomic>

#include "NoCopyable.h"
#include "Perf.h"

namespace Ser
{
class McsLock : NoCopyable
{
  public:
    void lock(McsLock **queue)
    {
        McsLock *prev;

        __atomic_store_n(&locked, 1, __ATOMIC_RELAXED);
        __atomic_store_n(&next, nullptr, __ATOMIC_RELAXED);

        prev = __atomic_exchange_n(queue, this, __ATOMIC_ACQ_REL);
        if (likely(prev == nullptr))
        {
            return;
        }
        __atomic_store_n(&prev->next, this, __ATOMIC_RELEASE);

        __atomic_thread_fence(__ATOMIC_ACQ_REL);

        while (__atomic_load_n(&locked, __ATOMIC_ACQUIRE) != 0)
        {
        }
    }

    void unlock(McsLock **queue, McsLock *m)
    {
        if (likely(__atomic_load_n(&next, __ATOMIC_RELAXED) == nullptr))
        {
            McsLock *me = __atomic_load_n(&m, __ATOMIC_RELAXED);

            if (likely(__atomic_compare_exchange_n(
                    queue, &me, nullptr, 0, __ATOMIC_RELEASE, __ATOMIC_RELAXED)))
                return;

            __atomic_thread_fence(__ATOMIC_ACQUIRE);
            uintptr_t *next;
            next = (uintptr_t *)&me->next;
            while (__atomic_load_n(next, __ATOMIC_RELAXED) != 0)
            {
                /* code */
            }
        }

        __atomic_store_n(&next->locked, 0, __ATOMIC_RELEASE);
    }

    bool trylock(McsLock **queue)
    {
        __atomic_store_n(&next, nullptr, __ATOMIC_RELAXED);

        McsLock *expected = nullptr;

        return __atomic_compare_exchange_n(
            queue, &expected, this, 0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED);
    }

  private:
    int locked;
    McsLock *next;
};

}  // namespace Ser

#endif  // _MCSLOCK_H_