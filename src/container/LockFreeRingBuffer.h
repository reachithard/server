#ifndef _LOCKFREERINGBUFFER_H_
#define _LOCKFREERINGBUFFER_H_

#include <stdint.h>
#include <tuple>
#include <optional>
#include <atomic>

#include "NoCopyable.h"
#include "Param.h"

namespace Ser {
// 之后把allocator抽象出来
enum class SyncType{
    SER_MUTIL,
    SER_SINGLE,
};

typedef struct HeadTailS {
  std::atomic<uint32_t> head = 0;
  std::atomic<uint32_t> tail = 0;
  SyncType sync = SyncType::SER_MUTIL;
} HeadTailT;

template <typename T, typename... Args>
class LockFreeRingBuffer : public NoCopyable {
public:
 static const bool hasCapacity = Detail::contains<uint32_t, Args...>::value;
public:
 LockFreeRingBuffer(/* args */) {}
 ~LockFreeRingBuffer() {}

private:
 HeadTailS prod;
 char pad1;
};
}

#endif // _LOCKFREERINGBUFFER_H_