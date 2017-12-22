
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>

typedef struct playground_struct {
    user_addr_t data;
    uint64_t size;
    void *kern_ptr;
} playground_struct;

#define ALLOC_BUFFER _IOWR('d', 0, playground_struct)
#define FREE_BUFFER _IOWR('d', 1, playground_struct)
#define REED_BUFFER _IOWR('d', 2, playground_struct)
#define WRITE_BUFFER _IOWR('d', 3, playground_struct)

int playground_init(void)
{
    int fd;

    if ((fd = open("/dev/kernel-heap-playground", O_RDONLY)) == -1 ){
        printf("error: couldn't open /dev/kernel-heap-playground\n");
        exit(1);
    }

    return fd;
}

void *playground_zalloc(int fd)
{
    playground_struct ps = { 0 };    
    ioctl(fd, ALLOC_BUFFER, &ps);
    return ps.kern_ptr;
}

void playground_zfree(int fd, void *mem)
{
    playground_struct ps = { 0 };
    ps.kern_ptr = mem;
    ioctl(fd, FREE_BUFFER, &ps);
}

void playground_read(int fd, void *kmem, void *dest, uint64_t size)
{
    playground_struct ps = { 0 };
    ps.data = (user_addr_t)dest;
    ps.size = size;
    ps.kern_ptr = kmem;
    ioctl(fd, REED_BUFFER, &ps);
}

void playground_write(int fd, void *kmem, void *src, uint64_t size)
{
    playground_struct ps = { 0 };
    ps.data = (user_addr_t)src;
    ps.size = size;
    ps.kern_ptr = kmem;
    ioctl(fd, WRITE_BUFFER, &ps);
}
