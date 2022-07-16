#ifndef _NOCOPYABLE_H_
#define _NOCOPYABLE_H_

namespace Ser
{
class NoCopyable
{
  protected:
    NoCopyable(){};
    ~NoCopyable(){};

    NoCopyable(const NoCopyable&) = delete;
    NoCopyable& operator=(const NoCopyable&) = delete;
};
}  // namespace Ser

#endif  // _NOCOPYABLE_H_