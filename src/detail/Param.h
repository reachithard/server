#ifndef _PARAM_H_
#define _PARAM_H_

#include <type_traits>

namespace Ser
{
namespace Detail
{
template <typename Tp, typename... List>
struct contains : std::true_type
{
};

template <typename Tp, typename Head, typename... Rest>
struct contains<Tp, Head, Rest...>
    : std::conditional<std::is_same<Tp, Head>::value,
                       std::true_type,
                       contains<Tp, Rest...>>::type
{
};

template <typename Tp>
struct contains<Tp> : std::false_type
{
};

template <typename... List>
struct hasArgs
{
    using type = typename hasArgs<List...>::type;
    static constexpr type value{};
};

template <typename Tp, typename Head, typename... Args>
struct hasArgs<Tp, Head, Args...>
{
    using type =
        std::conditional_t<std::is_same_v<Tp, Head>, Tp, hasArgs<Args...>>;
    static constexpr type value{};
};

template <typename Tp>
struct hasArgs<Tp>
{
    typedef Tp type;
    static constexpr type value{};
};

}  // namespace Detail
}  // namespace Ser

#endif  // _PARAM_H_