//
//  kernel_heap_playground.c
//  kernel-heap-playground
//
//  Created by Simone Ferrini on 04/12/2017.
//  Copyright © 2017 Simone Ferrini. All rights reserved.
//

#include <mach/mach_types.h>
#include <mach-o/loader.h>

#include <kern/host.h>
#include <libkern/libkern.h>
#include <miscfs/devfs/devfs.h>

#include <sys/conf.h>
#include <sys/ioctl.h>
#include <sys/systm.h>

#include "common.h"
#include "sym.h"

kern_return_t kernel_heap_playground_start(kmod_info_t *ki, void *d);
kern_return_t kernel_heap_playground_stop(kmod_info_t *ki, void *d);

static int device_major;
static void *device_handle;
static const char *device_name = "kernel-heap-playground";

typedef struct playground_struct {
    user_addr_t data;
    uint64_t size;
    void *kern_ptr;
} playground_struct;

#define ALLOC_BUFFER _IOWR('d', 0, playground_struct)
#define FREE_BUFFER _IOWR('d', 1, playground_struct)
#define REED_BUFFER _IOWR('d', 2, playground_struct)
#define WRITE_BUFFER _IOWR('d', 3, playground_struct)

typedef void *zone_t;

#define BUFFSIZE 64
#define MAXBUFFS 128

zone_t heap_playground_zone;

zone_t (*_zinit)(vm_size_t size, vm_size_t max, vm_size_t alloc, const char *name);
void *(*_zalloc)(zone_t zone);
void (*_zfree)(zone_t zone, void *addr);

static int heap_playground_ioctl(dev_t dev, u_long cmd, caddr_t data, int flag, struct proc *p)
{
    playground_struct *ps = (playground_struct *)data;
    
    switch (cmd) {
        case ALLOC_BUFFER:
            LOG("Allocating block...");
            
            void *buffer = _zalloc(heap_playground_zone);
            LOG_PTR("zalloc block:", buffer);
            
            if (!buffer) {
                LOG("error: could not allocate buffer\n");
                return KERN_FAILURE;
            }
            
            ps->kern_ptr = buffer;
        break;
            
        case FREE_BUFFER:
            LOG("Freeing block...\n");
            _zfree(heap_playground_zone, ps->kern_ptr);
            ps->kern_ptr = NULL;
        break;
        
        case REED_BUFFER:
            if (copyout(ps->kern_ptr, ps->data, ps->size) == -1) {
                LOG("error: copyout failed.\n");
                return KERN_FAILURE;
            }
        break;
        
        case WRITE_BUFFER:
            if (copyin(ps->data, ps->kern_ptr, ps->size) == -1) {
                LOG("error: copyin failed.\n");
                return KERN_FAILURE;
            }
        break;
            
        default:
            LOG("error: bad ioctl cmd\n");
        break;
    }
    
    return KERN_SUCCESS;
}

static int heap_playground_open_close(dev_t dev, int flags, int type, struct proc *p)
{
    return KERN_SUCCESS;
}

static struct cdevsw device_fops = {
    .d_open = heap_playground_open_close,
    .d_close = heap_playground_open_close,
    .d_read = eno_rdwrt,
    .d_write = eno_rdwrt,
    .d_ioctl = heap_playground_ioctl,
    .d_stop = eno_stop,
    .d_reset = eno_reset,
    .d_ttys = NULL,
    .d_select = eno_select,
    .d_mmap = eno_mmap,
    .d_strategy = eno_strat,
    .d_reserved_1 = NULL,
    .d_reserved_2 = NULL,
    .d_type = D_TTY
};

kern_return_t kernel_heap_playground_start(kmod_info_t *ki, void *d)
{
    LOG("Initializing function pointers...");
    
    host_priv_t host = host_priv_self();
    LOG_PTR("realhost", host);
    if (!host) return KERN_RESOURCE_SHORTAGE;
    
    uintptr_t base;
    for (base = ((uintptr_t)host) & 0xfffffffffff00000; *(uint32_t *)base != MH_MAGIC_64; base -= 0x100000);
    LOG_PTR("kernel base", base);
    
    SYM(zinit);
    SYM(zalloc);
    SYM(zfree);
    
    LOG("Initializing zalloc zone...");
    
    heap_playground_zone = _zinit(BUFFSIZE, (BUFFSIZE * MAXBUFFS) + BUFFSIZE, 0, device_name);
    
    if (heap_playground_zone == NULL) {
        LOG("zinit failed\n");
        return KERN_FAILURE;
    }
    
    LOG("Initializing device...");
    
    device_major = cdevsw_add(-1, &device_fops);
    
    if (device_major < 0) {
        LOG("cdevsw_add failed\n");
        return KERN_FAILURE;
    }
    
    device_handle = devfs_make_node(makedev(device_major, 0), DEVFS_CHAR, 0, 0, 0777, "%s", device_name);
    
    if (device_handle == NULL) {
        LOG("devfs_make_node failed\n");
        return KERN_FAILURE;
    }
    
    return KERN_SUCCESS;
}

kern_return_t kernel_heap_playground_stop(kmod_info_t *ki, void *d)
{
    if (device_handle != NULL) {
        devfs_remove(device_handle);
    }
    
    if (device_major != -1) {
        cdevsw_remove(device_major, &device_fops);
    }
    return KERN_SUCCESS;
}
