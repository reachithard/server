#ifndef _PERF_H_
#define _PERF_H_

#include <chrono>
#include <iostream>

#include "NoCopyable.h"

namespace Ser
{
enum class PerfTimeType
{
    SECOND,
    MILLISECOND,
    MICROSECOND
};

class Perf : public NoCopyable
{
  public:
    Perf()
    {
    }
    ~Perf()
    {
    }

    void Start()
    {
        start = std::chrono::steady_clock::now();
    }

    void End(PerfTimeType type = PerfTimeType::SECOND)
    {
        end = std::chrono::steady_clock::now();
        switch (type)
        {
            case PerfTimeType::MILLISECOND:
                /* code */
                {
                    auto millisecond =
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            end.time_since_epoch() - start.time_since_epoch())
                            .count();
                    std::cout << "elapesd total second:" << millisecond << "ms"
                              << std::endl;
                }

                break;
            case PerfTimeType::MICROSECOND:
                /* code */
                {
                    auto microsecond =
                        std::chrono::duration_cast<std::chrono::microseconds>(
                            end.time_since_epoch() - start.time_since_epoch())
                            .count();
                    std::cout << "elapesd total second:" << microsecond << "us"
                              << std::endl;
                }

                break;
            default:
                /* code */
                {
                    double second =
                        std::chrono::duration<double>(end.time_since_epoch() -
                                                      start.time_since_epoch())
                            .count();
                    std::cout << "elapesd total second:" << second << "s"
                              << std::endl;
                }
                break;
        }
    }

  private:
    std::chrono::_V2::steady_clock::time_point start;
    std::chrono::_V2::steady_clock::time_point end;
};
}  // namespace Ser

#endif  // _PERF_H_