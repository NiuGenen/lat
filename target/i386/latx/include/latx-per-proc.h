#ifndef _LATX_PER_PROC_H_
#define _LATX_PER_PROC_H_

//#define LATX_PERFMAP_ENABLE
//#define LATX_PRINT_PER_THREAD_ENABLE

#ifdef LATX_PERFMAP_ENABLE
void latx_perfmap_insert(void *start, long len, const char *name);
void latx_perfmap_flush(void);
#else
static inline void latx_perfmap_insert(void *s, long l, const char *n) {}
static inline void latx_perfmap_flush(void) {}
#endif

#ifdef LATX_PRINT_PER_THREAD_ENABLE
extern __thread FILE *latx_print_per_thread_file;
void latx_print_perthread_init(void);
static inline unsigned long latx_perthread_time(void)
{
    unsigned long rd = 0, rj = 0;
    __asm__ __volatile__ ("rdtime.d %0, %1\r\n":"=r"(rd):"r"(rj):);
    return rd;
}
#define latx_perthread_print(fmt, ...) do {                         \
    latx_print_perthread_init();                                    \
    if (latx_print_per_thread_file) {                               \
        fprintf(latx_print_per_thread_file, fmt, __VA_ARGS__);      \
        fflush(latx_print_per_thread_file);                         \
    }                                                               \
} while (0)
#define latx_perthread_print_time(fmt, ...) do {                    \
    latx_print_perthread_init();                                    \
    if (latx_print_per_thread_file) {                               \
        fprintf(latx_print_per_thread_file,                         \
                "[%ld]", latx_perthread_time());                    \
        fprintf(latx_print_per_thread_file, fmt, __VA_ARGS__);      \
        fflush(latx_print_per_thread_file);                         \
    }                                                               \
} while (0)
#else
#define latx_perthread_print(fmt, ...)
#define latx_perthread_print_time(fmt, ...)
#endif

#endif
