#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#ifdef __linux__ 
    #include <unistd.h>
#else  
    #error "Not supported"
#endif 

#define CWSTR(str) ((const void*)str), (sizeof str)

static inline uint64_t s_x86_64__u64log2(uint64_t x) {
    uint64_t y;
    asm ( "\tbsr %1, %0\n"
        : "=r"(y)
        : "r" (x)
    );

    return y;
}

#ifdef __x86_64__ 
    #define s__u64log2(x) s_x86_64__u64log2(x)
#else 
    #error "Not supported"
#endif 

// Using this on systems with platform widths > 64 not recommended.
int s__u64log10(uint64_t x) {
    int t;
    int r;

    static uint64_t PowersOf10[] = {
        1, 10, 100, 1000, 10000, 100000, 1000000, 10000000,
        100000000, 1000000000, 10000000000, 100000000000,
        100000000000, 1000000000000, 10000000000000,
        100000000000000, 1000000000000000, 
        10000000000000000, 100000000000000000, 
        1000000000000000000, 10000000000000000000UL,
    };

    t = (s__u64log2(x) + 1) * 1233 >> 12;
    r = t - (x < PowersOf10[t]);

    return r;
}

struct s__u64toabuf {
    char bytes[28];
    int len;
};

struct s__u64toabuf s__u64toa(uint64_t i) {
    struct s__u64toabuf buf = {};
    int bw_it = s__u64log10((uint64_t)i) + 1;
    buf.len = bw_it;

    while (bw_it > 0) {
        buf.bytes[--bw_it] = (char)(i % 10) + '0';
        i /= 10;
    }

    return buf;
}

void* safealloc(size_t nmemb, size_t size) {
    if (nmemb == 0 || size == 0)
        return NULL; // man malloc 

    void* mem = calloc(nmemb, size);

    if (!mem) {
        uint64_t bytes = ((uint64_t)nmemb) * size;
        struct s__u64toabuf u64str = s__u64toa(bytes);

        write(fileno(stderr), CWSTR("Failed to allocate "));
        write(fileno(stderr), u64str.bytes, u64str.len);
        write(fileno(stderr), CWSTR(" bytes in " __FILE__ "\n"));
    }

    return mem;
}
