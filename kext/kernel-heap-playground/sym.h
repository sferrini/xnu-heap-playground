#ifndef SYM_H
#define SYM_H

#include <stdint.h>             // uintptr_t

uintptr_t find_sym(uintptr_t base, const char *name);

#define SYM(s) \
do { \
    uintptr_t _addr = find_sym(base, #s); \
    if (!_addr) { \
        return KERN_RESOURCE_SHORTAGE; \
    } \
    LOG_PTR("sym(" #s ")", _addr); \
    _##s = (void*)_addr; \
} while(0)

#endif
