#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int view_hex(char path[], size_t limit, char out_path[]);

int main(int argc, char *argv[])
{
    int opt;
    int limit = -1;
    
    size_t out_size = 32;
    char *out_path = malloc(out_size);
    if (!out_path)
    {
        printf("Memory allocation failed\n");
        return -1;
    }
    out_path[0] = '\0';

    while ((opt = getopt(argc, argv, "l:o:")) != -1)
    {
        switch (opt)
        {
        case 'l':
            limit = (unsigned int)strtol(optarg, NULL, 16) + 0x10;
            break;
        case 'o':
            if (strlen(optarg) + 1 > out_size)
            {
                char *new_out_path = realloc(out_path, strlen(optarg) + 1);
                if (!new_out_path)
                {
                    printf("Memory allocation failed\n");
                    free(out_path);
                    return -1;
                }
                out_path = new_out_path;
                out_size = strlen(optarg) + 1;
            }
            strcpy(out_path, optarg);
            break;
        default:
            fprintf(stderr, "Options:\n-l <limit>  Limit the number of bytes to read\n-o <path>   Output path\n");
            free(out_path);
            return -1;
        }
    }

    if (optind >= argc)
    {
        fprintf(stderr, "Expected file argument after options\n");
        free(out_path);
        return -1;
    }

    char *file_path = argv[optind];
    int result = view_hex(file_path, limit, out_path);
    free(out_path);
    return result;
}

int output_file(const char *str, const char *out_path)
{
    FILE *fptr = fopen(out_path, "a");
    if (!fptr)
    {
        return -1;
    }

    fputs(str, fptr);
    fclose(fptr);
    return 0;
}

int view_hex(char path[], size_t limit, char out_path[])
{
    FILE *fptr;
    size_t bytes_read;
    size_t offset = 0;
    size_t buffer_size = 16;

    char line_output[128];  // Buffer to hold entire line output
    char ascii_string[17];
    ascii_string[16] = '\0';  // Ensure null termination

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
                snprintf(line_output, sizeof(line_output), "%08X  ", offset);
            }

            snprintf(line_output + strlen(line_output), sizeof(line_output) - strlen(line_output), "%02X ", buffer[i]);

            ascii_string[i % 16] = (buffer[i] >= 32 && buffer[i] <= 126) ? buffer[i] : '.';

            offset++;

            if (offset % 16 == 0 || offset == limit)
            {
                snprintf(line_output + strlen(line_output), sizeof(line_output) - strlen(line_output), "  %s\n", ascii_string);
                printf("%s", line_output);
                
                if (out_path[0] != '\0')
                    output_file(line_output, out_path);
            }

            if (offset == limit)
                break;
        }
        if (offset == limit)
            break;
    }

    if (offset % 16 != 0)
    {
        // Handle last line padding
        int padding = (16 - (offset % 16)) * 3;
        snprintf(line_output + strlen(line_output), sizeof(line_output) - strlen(line_output), "%*s  %s\n", padding, "", ascii_string);
        printf("%s", line_output);

        if (out_path[0] != '\0')
            output_file(line_output, out_path);
    }

    fclose(fptr);
    free(buffer);
    return 0;
}