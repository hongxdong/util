
#if !defined(__GNUC__) || (__GNUC__ == 2 && __GNUC_MINOR__ < 96)

#define likely(x)       if((x))
#define unlikely(x)     if((x))

#else

#define likely(x)       if(__builtin_expect(!!(x), 1))
#define unlikely(x)     if(__builtin_expect((x), 0))

#endif
