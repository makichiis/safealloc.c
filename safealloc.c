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

const char* s__ulltoa(unsigned long long i) {
    static char buf[24] = {};
    int bw_it = s__u64log10((uint64_t)i) + 1;

    while (bw_it > 0) {
        buf[--bw_it] = (char)(i % 10) + '0';
        i /= 10;
    }

    return buf;
}

void* safealloc(size_t nmemb, size_t size) {
    if (nmemb == 0 || size == 0)
        return NULL; // man malloc 

    void* mem = calloc(nmemb, size);

    if (!mem) {
        unsigned long long bytes = nmemb * size;
        const char* bs = s__ulltoa(bytes);
        int bslen = s__u64log10(bytes) + 2;

        write(fileno(stderr), CWSTR("Failed to allocate "));
        write(fileno(stderr), bs, bslen);
        write(fileno(stderr), CWSTR(" bytes in " __FILE__ "\n"));
    }

    return mem;
}
