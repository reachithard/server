#ifndef _RUNTIME_H_
#define _RUNTIME_H_

#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#include <thread>
#include <mutex>

#include "Singleton.h"

namespace Ser
{
class Runtime : public Singleton<Runtime>
{
  public:
    Runtime()
    {
    }

    ~Runtime()
    {
    }

    int Init()
    {
        return CreateDir();
    }

    int CreateDir()
    {
        const char *directory = getenv("SER_RUNTIME_DIRECTORY");
        if (directory == nullptr)
        {
            runtime = ".";
        }
        else
        {
            runtime = std::string(directory);
        }

        int ret = 0;
        serDir = runtime + "/ser/";
        ret = mkdir(serDir.c_str(), 0700);
        if (ret < 0 && errno != EEXIST)
        {
            return -1;
        }

        ringDir = runtime + "/ser/ring";
        ret = mkdir(ringDir.c_str(), 0700);
        if (ret < 0 && errno != EEXIST)
        {
            return -1;
        }
        return 0;
    }

    std::string GetRuntimeDir() const
    {
        return runtime;
    }

    std::string GetSerDir() const
    {
        return serDir;
    }

    std::string GetRingDir() const
    {
        return ringDir;
    }

  private:
    std::string runtime;
    std::string serDir;
    std::string ringDir;
    std::once_flag flag;
};

}  // namespace Ser

#endif  // _RUNTIME_H_