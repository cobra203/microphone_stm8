#ifndef STM8S_COMMON_H
#define STM8S_COMMON_H

#define     BIT_ISSET(a, s)     (((a) >> (s)) & 0x1)
#define     BIT_SET(a, s)       ((a) = (a) | 0x1 << (s))
#define     BIT_CLR(a, s)       ((a) = (a) & ~(0x1 << (s)))

#endif /* STM8S_COMMON_H */
