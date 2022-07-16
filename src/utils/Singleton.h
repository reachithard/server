#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include "NoCopyable.h"

namespace Ser
{
template <typename T>
class Singleton : public NoCopyable
{
  public:
    static T &Instance()
    {
        static T single;
        return single;
    }

  protected:
    Singleton(/* args */)
    {
    }
    ~Singleton()
    {
    }
};
}  // namespace Ser

#endif  // _SINGLETON_H_