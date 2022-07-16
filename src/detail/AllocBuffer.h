#ifndef _ALLOCBUFFER_H_
#define _ALLOCBUFFER_H_

#include <stdint.h>
#include <cstddef>
#include <type_traits>
#include <memory>
#include <cstring>

namespace Ser
{
namespace Detail
{
template <typename T, typename Alloc = std::allocator<T> >
struct RuntimeStorage
{
    T* nodes;
    std::size_t cnt;
    Alloc alloc;

    RuntimeStorage(std::size_t count) : cnt(count)
    {
        nodes = alloc.allocate(cnt);
        std::memset(nodes, 0, sizeof(T) * cnt);
    }

    ~RuntimeStorage()
    {
        alloc.deallocate(nodes, cnt);
    }

    T* getNodes() const
    {
        return nodes;
    }

    std::size_t getCnt() const
    {
        return cnt;
    }
};

// template <typename T, typename Alloc, bool isSized, bool isFixed,
//           std::size_t capacity>
// struct SelectStorage {
//   typedef typename std::conditional_t<isSized, void_t, void_t>
//   fiexdStorageType; typedef typename std::conditional_t<isSized || isFixed,
//   void_t, void_t>
//       fiexdStorageType;
// };
}  // namespace Detail
}  // namespace Ser

#endif  // _ALLOCBUFFER_H_