#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int view_hex(char path[], size_t limit);

int main(int argc, char *argv[])
{
    int limit = -1;
    int opt;

    while ((opt = getopt(argc, argv, "l:")) != -1)
    {
        switch (opt)
        {
        case 'l':
            limit = (unsigned int)strtol(optarg, NULL, 16) + 0x10;
            break;
        default:
            fprintf(stderr, "Usage: %s [-l limit <file>\n]", argv[0]);
            return -1;
        }
    }

    if (optind >= argc)
    {
        fprintf(stderr, "Expected file argument after options\n");
        return -1;
    }

    char *file_path = argv[optind];

    return view_hex(file_path, limit);
}

int view_hex(char path[], size_t limit)
{
    FILE *fptr;
    size_t bytes_read;
    size_t offset = 0;
    size_t buffer_size = 16;
    char ascii_string[17];

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
                printf("%08X   ", offset);
            }

            printf("%02X ", buffer[i]);
            ascii_string[i % 16] = (buffer[i] >= 32 && buffer[i] <= 126) ? buffer[i] : '.';

            offset++;

            if (offset % 16 == 0)
            {
                printf("  %s\n", ascii_string);
            }
        }
        if (offset == limit)
        {
            break;
        }
    }

    if (offset % 16 != 0)
    {
        int padding = (16 - (offset % 16)) * 3;
        printf("%*s  ", padding, "");

        ascii_string[offset % 16] = '\0';
        printf("%s\n", ascii_string);
    }

    fclose(fptr);
    free(buffer);
    return 0;
}