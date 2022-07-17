#ifndef _TICKETLOCK_H_
#define _TICKETLOCK_H_

#include <atomic>

#include "NoCopyable.h"

namespace Ser {
    // 比起自旋锁特点是谁先到谁获得
    class TicketLock : public NoCopyable {
        public:
            void lock() {
                uint16_t seq =
                    next.fetch_add(1, std::memory_order_relaxed);
                while (cur.load(std::memory_order_acquire) != seq)
                {
                    /* code */
                    // 不等于就空转
                }
            }

            void unlock() {
                uint16_t tmp = cur.load(std::memory_order_relaxed);
                cur.store(tmp + 1, std::memory_order_release);
            }

            private:
              /* data */
              std::atomic<uint16_t> cur{0};
              std::atomic<uint16_t> next{0};
    };
}


#endif  // _TICKETLOCK_H_