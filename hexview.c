#include <stdio.h>
#include <stdlib.h>

int view_hex(char path[]);

void main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <file_path>\n", argv[0]);
        return;
    }

    view_hex(argv[1]);
}

int view_hex(char path[])
{
    FILE *fptr;
    size_t bytes_read;
    size_t offset = 0;
    size_t buffer_size = 16;

    unsigned char *buffer = malloc(buffer_size);
    if (!buffer)
    {
        printf("Memory allocation failed\n");
        return -1;
    }

    fptr = fopen(path, "rb");
    if (!fptr)
    {
        printf("%s not found.\n", path);
        free(buffer);
        return -1;
    }

    while ((bytes_read = fread(buffer, 1, buffer_size, fptr)) > 0)
    {
        for (size_t i = 0; i < bytes_read; i++)
        {
            if (offset % 16 == 0)
            {
                printf("%08zX   ", offset);
            }
            printf("%02X ", buffer[i]);
            offset++;
            if (offset % 16 == 0)
            {
                printf("\n");
            }
        }
    }

    if (offset % 16 != 0)
    {
        printf("\n");
    }

    fclose(fptr);
    free(buffer);
    return 0;
}