#ifndef _BARRIER_H_
#define _BARRIER_H_

#define ser_mb()                       \
    do                                 \
    {                                  \
        asm volatile("" ::: "memory"); \
    } while (0)

#define ser_wmb()                      \
    do                                 \
    {                                  \
        asm volatile("" ::: "memory"); \
    } while (0)

#define ser_rmb()                      \
    do                                 \
    {                                  \
        asm volatile("" ::: "memory"); \
    } while (0)

#define ser_smp_mb() ser_mb()

#define ser_smp_wmb() ser_wmb()

#define ser_smp_rmb() ser_rmb()

#define ser_io_mb() ser_mb()

#define ser_io_wmb() ser_wmb()

#define ser_io_rmb() ser_rmb()

#endif  // _BARRIER_H_