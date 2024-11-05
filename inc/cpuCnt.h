#include <stdint.h>

#ifdef _MSC_VER
#include <intrin.h>  // Für MSVC

static inline uint64_t rdtsc() {
    return __rdtsc();  // MSVC-spezifische Funktion
}

#else
static inline uint64_t rdtsc() {
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}
#endif
