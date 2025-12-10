#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/syscall.h>
#include "./include/latx-per-proc.h"

#ifdef LATX_PERFMAP_ENABLE
static FILE *latx_perfmap;
static char latx_perfmap_data[4096];
static int latx_perfmap_index;

static void perfmap_main_init(void)
{
    char perfmap_file[64];
    sprintf(perfmap_file, "/tmp/perf-%d.map", getpid());
    latx_perfmap = fopen(perfmap_file, "w");
    if (latx_perfmap == NULL) {
        fprintf(stderr, "perfmap open fail. output to stderr.\n");
    }
    latx_perfmap_index = 0;
}

static void perfmap_child_init(void)
{
    char perfmap_file[64];
    sprintf(perfmap_file, "/tmp/perf-%d.map", getpid());
    latx_perfmap = fopen(perfmap_file, "w");
    if (latx_perfmap_index != 0) {
        fprintf(latx_perfmap, "%s", latx_perfmap_data);
        fflush(latx_perfmap);
    }
}

void latx_perfmap_insert(void *start, long len, const char *name)
{
    int size = 0;
    if (latx_perfmap) {
        fprintf(latx_perfmap, "%p 0x%lx %s\n", start, len, name);
    } else {
        fprintf(stderr, "%p 0x%lx %s\n", start, len, name);
    }
    size = sprintf(&latx_perfmap_data[latx_perfmap_index],
            "%p 0x%lx %s\n", start, len, name);
    latx_perfmap_index += size;
    latx_perfmap_flush();
}

void latx_perfmap_flush(void)
{
    if (latx_perfmap) {
        fflush(latx_perfmap);
    }
}
#endif


#ifdef LATX_PRINT_PER_THREAD_ENABLE
__thread FILE *latx_print_per_thread_file;
static inline void print_perthread_file_create(void)
{
    char file[64];
    sprintf(file, "/tmp/latx-perthread-%d-%d.data",
            getpid(), (int)syscall(SYS_gettid));
    latx_print_per_thread_file = fopen(file, "w");
}
static void print_perthread_main_init(void)
{
    print_perthread_file_create();
}
static void print_perthread_child_init(void)
{
    print_perthread_file_create();
}
void latx_print_perthread_init(void)
{
    if (latx_print_per_thread_file) return;
    print_perthread_file_create();
}
#endif

static void child_per_proc_init(void)
{
#ifdef LATX_PERFMAP_ENABLE
    perfmap_child_init();
#endif
#ifdef LATX_PRINT_PER_THREAD_ENABLE
    print_perthread_child_init();
#endif
}

static
void __attribute__((__constructor__)) latx_per_proc_init(void)
{
    int need_child = 0;
#ifdef LATX_PERFMAP_ENABLE
    perfmap_main_init();
    need_child = 1;
#endif
#ifdef LATX_PRINT_PER_THREAD_ENABLE
    print_perthread_main_init();
    need_child = 1;
#endif
    if (need_child) {
        pthread_atfork(NULL, NULL, child_per_proc_init);
    }
}
