
#ifndef HEAP_PLAYGROUND_HEADER
#define HEAP_PLAYGROUND_HEADER

int playground_init(void);

void *playground_zalloc(int fd);
void playground_zfree(int fd, void *mem);

void playground_read(int fd, void *kmem, void *dest, uint64_t size);
void playground_write(int fd, void *kmem, void *src, uint64_t size);

#endif // HEAP_PLAYGROUND_HEADER
