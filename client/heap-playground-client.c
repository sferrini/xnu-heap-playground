
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "heap-playground.h"
#include "utils.h"

#define PLAYGROUND_CLI_BUFFER_SIZE (1000)

void help(void)
{
    puts("Commands: zalloc, zfree p, read p size, write p size (content 0x41).\n");
}

int main(int ac, char **av)
{
    int fd = playground_init();

    char *buffer = (char *)malloc(PLAYGROUND_CLI_BUFFER_SIZE);
    char *cmd = (char *)malloc(PLAYGROUND_CLI_BUFFER_SIZE);

    help();

    while (1) {
        fprintf(stderr, "> ");
        fgets(buffer, PLAYGROUND_CLI_BUFFER_SIZE, stdin);

        void *kmem = NULL;
        uint64_t size = 0;
        sscanf(buffer, "%s %p %lld", cmd, &kmem, &size);

        if (strcmp(cmd, "zalloc") == 0) {
            void *result = playground_zalloc(fd);
            fprintf(stderr, "==> %p\n", result);
        } else if (strcmp(cmd, "zfree") == 0) {
            playground_zfree(fd, kmem);
            fprintf(stderr, "==> ok\n");
        } else if (strcmp(cmd, "read") == 0) {
            void *dest = malloc(size);
            playground_read(fd, kmem, dest, size);
            hex_dump(dest, size);
        } else if (strcmp(cmd, "write") == 0) {
            void *src = malloc(size);
            memset(src, 0x41, size);
            playground_write(fd, kmem, src, size);
            fprintf(stderr, "==> ok\n");
        } else {
            help();
        }
    }
}
