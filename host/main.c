#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "inc.h"

static int write_startup(FILE* startup, FILE* out) {
    struct stat file_stat;
    if (fstat( fileno(startup), &file_stat ) != 0) {
        return -1;
    }
    void* buffer = malloc(file_stat.st_size);
    if (!buffer) return -1;

    fread(buffer, 1, file_stat.st_size, startup);
    fwrite(buffer, 1, file_stat.st_size, out);

    free(buffer);
    return 0;
}

static int write_exe(FILE* exe, FILE* out) {
    struct stat file_stat;
    if (fstat( fileno(exe), &file_stat ) != 0) {
        return -1;
    }
    void* buffer = malloc(file_stat.st_size);
    if (!buffer) return -1;

    fread(buffer, 1, file_stat.st_size, exe);
    fwrite(buffer, 1, file_stat.st_size, out);

    free(buffer);
    return 0;
}

static int write_offsets(uint32_t* list, uint32_t count, FILE* out) {
    fwrite(list, 4, count, out);

    fseek(out, 8, SEEK_SET);
    uint32_t count_endian_fixed = htole32(count);
    fwrite(&count_endian_fixed, 1, 4, out);

    fseek(out, 0, SEEK_END);
}

int main(int argc, char *argv[]) {
    int ret = 0;
    if (argc != 5) {
        printf(
            "ARM ELF relocation fixer\n"
            "Usage: %s inputelf inputbinary outputfile /path/to/startup.bin\n"
            ,argv[0]);
        return 0;
    }
    int fd = open(argv[1],O_RDONLY);
    if (fd < 0) {
        printf("%s: Unable to open \"%s\"\n", argv[0], argv[1]);
        goto error;
    }

    FILE *startup = NULL, *inbin = NULL, *out = NULL;

    if ( !(inbin = fopen(argv[2],"rb")) ) {
        printf("%s: Unable to open \"%s\"\n", argv[0], argv[2]);
        goto error;
    }

    if ( !(out = fopen(argv[3],"wb")) ) {
        printf("%s: Unable to open \"%s\"\n", argv[0], argv[3]);
        goto error;
    }

    if ( !(startup = fopen(argv[4],"rb")) ) {
        printf("%s: Unable to open \"%s\"\n", argv[0], argv[4]);
        goto error;
    }


    uint32_t count;
    uint32_t * offsets = elf_get_offsets(fd, &count);

    if (!offsets) {
        printf("%s: Unable to parse ELF file \"%s\"\n", argv[0], argv[1]);
        goto error;
    }

    write_startup(startup, out);
    write_offsets(offsets, count, out);
    write_exe(inbin, out);

    free(offsets);

    goto success;

    error:
    ret = -1;

    success:
    if (fd >= 0) close(fd);
    fclose(startup);
    fclose(out);
    fclose(inbin);
    return ret;
}