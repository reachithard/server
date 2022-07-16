#ifndef _SHAALLOCATOR_H_
#define _SHAALLOCATOR_H_

// 提供mmap函数
#include <sys/mman.h>

#include <cstddef>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Runtime.h"

namespace Ser
{
template <typename T = char>
class ShaAllocator
{
  public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

  private:
    /* data */
  public:
    ShaAllocator(/* args */)
    {
        // 这里之后改为业务层初始化
        Singleton<Runtime>::Instance().Init();
    }
    ~ShaAllocator()
    {
    }

    // 这一类是匿名映射
    T *allocate(size_type n, const void *hind = 0)
    {
        int prot = PROT_READ | PROT_WRITE;

        int flag = MAP_SHARED | MAP_ANON;

        T *buffer =
            (T*)mmap(NULL, sizeof(T) * n, prot, MAP_SHARED | MAP_ANON, -1, 0);

        if (buffer == MAP_FAILED)
        {
            printf("map failed! %s\n", strerror(errno));
            return nullptr;
        }

        return buffer;
    }

    T *allocate(std::string key,
                uint64_t id,
                size_type n,
                size_type header = 0, const void *hidt = 0)
    {
        std::string pathname =
            Singleton<Runtime>::Instance().GetRingDir() + "/" +  key + "_" + std::to_string(id);
        int tmpfd = open(pathname.c_str(), O_RDWR | O_CREAT, 0600);
        if (tmpfd < 0)
        {
            return nullptr;
        }

        size_t length = header + sizeof(T) * n; // 前半部分
        int retval = ftruncate(tmpfd, length);
        if (retval < 0)
        {
            close(tmpfd);
            return nullptr;
        }

        int prot = PROT_READ | PROT_WRITE;
        int flag = MAP_SHARED;

        T *buffer = (T *)mmap(nullptr, 0, prot, flag, tmpfd, 0);
        if (buffer == MAP_FAILED)
        {
            munmap(buffer, length);
            close(tmpfd);
            return nullptr;
        }

        close(tmpfd);
        return buffer;
    }

    void deallocate(std::string key,
                    uint64_t id,
                    T *p,
                    size_type n, size_type header = 0)
    {
        std::string pathname = Singleton<Runtime>::Instance().GetRingDir() +
                               "/" + key + "_" + std::to_string(id);
        unlink(pathname.c_str());
        if (p != nullptr)
        {
            munmap(p, sizeof(T) * n + header);
        }
    }

    void deallocate(T *p, size_type n)
    {
        if (p != nullptr)
        {
            munmap(p, sizeof(T) * n);
        }
    }
};
}  // namespace Ser

#endif  // _SHAALLOCATOR_H_