#ifndef DEFS_H
#define DEFS_H

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define UNUSED __attribute__((unused))

#endif // DEFS_H
