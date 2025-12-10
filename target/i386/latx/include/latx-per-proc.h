#ifndef _LATX_PER_PROC_H_
#define _LATX_PER_PROC_H_

//#define LATX_PERFMAP_ENABLE

#ifdef LATX_PERFMAP_ENABLE
void latx_perfmap_insert(void *start, long len, const char *name);
void latx_perfmap_flush(void);
#else
static inline void latx_perfmap_insert(void *s, long l, const char *n) {}
static inline void latx_perfmap_flush(void) {}
#endif

#endif
