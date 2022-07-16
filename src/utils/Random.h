#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <random>
#include <functional>
#include <map>
#include <string>
#include <cassert>


namespace Ser {
inline unsigned int Rand()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned int> dis(
        0, std::numeric_limits<unsigned int>::max());
    return dis(gen);
}

template <typename T>
inline T RandRange(T min, T max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<T> dis(min, max);
    return dis(gen);
}

template <class T = double>
inline T RandDoubleRange(T min, T max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<T> dis(min, max);
    return dis(gen);
}

inline bool RandPercent(double percent)
{
    if (percent <= 0.0)
    {
        return false;
    }
    if (RandDoubleRange(0.0f, 1.0f) >= percent)
    {
        return false;
    }
    return true;
}

inline const unsigned char* RandKey(size_t len = 8)
{
    static thread_local unsigned char tmp[256];
    assert(len < sizeof(tmp));
    char x = 0;
    for (size_t i = 0; i < len; ++i)
    {
        tmp[i] = static_cast<unsigned char>(rand() & 0xFF);
        x ^= tmp[i];
    }

    if (x == 0)
    {
        tmp[0] |= 1;  // avoid 0
    }

    return tmp;
}

template <typename Values, typename Weights>
inline auto RandWeight(const Values& v, const Weights& w)
{
    if (v.empty() || v.size() != w.size())
    {
        return -1;
    }

    auto dist = std::discrete_distribution<int>(w.begin(), w.end());
    auto g = std::mt19937(std::random_device{}());
    int index = dist(g);
    return v[index];
}
}

#endif // _RANDOM_H_