#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <libelf/libelf.h>
#include <libelf/gelf.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "inc.h"

#define R_ARM_ABS32 2

#define DEFAULT_REALLOC_INCREASE 4*16

struct uint32_array {
    uint32_t * ptr;
    size_t size;
    size_t curr_index;
};

static struct uint32_array * array_alloc() {
    struct uint32_array * alloc = malloc(sizeof(struct uint32_array));
    if (!alloc) return NULL;

    alloc->ptr = malloc(DEFAULT_REALLOC_INCREASE);
    if (!alloc->ptr) { free(alloc); return NULL; }
    alloc->size = DEFAULT_REALLOC_INCREASE/sizeof(uint32_t);
    alloc->curr_index = 0;

    return alloc;
}

static struct uint32_array * array_add(struct uint32_array * alloc, uint32_t val) {
    if (!alloc) return alloc;

    if (alloc->curr_index >= alloc->size) {
        size_t new_size = (alloc->size*sizeof(uint32_t)) + DEFAULT_REALLOC_INCREASE;
        alloc->size = new_size/4;
        alloc->ptr = realloc(alloc->ptr, alloc->size);
        if (!alloc->ptr) { free(alloc); return NULL; }
    }

    alloc->ptr[alloc->curr_index] = val;
    alloc->curr_index++;
    return alloc;
}

static void array_free(struct uint32_array * alloc) {
    free(alloc);
}
static void array_free_all(struct uint32_array * alloc) {
    free(alloc->ptr);
    free(alloc);
}

uint32_t* elf_get_offsets(int fd, uint32_t* count) {
    struct uint32_array* array_data = array_alloc();
    if (!array_data) return NULL;

    if ( elf_version ( EV_CURRENT ) == EV_NONE ) {
        goto cleanupfail;
    }

    Elf *ar_e = elf_begin(fd, ELF_C_READ, NULL);
    if (!ar_e) {
        goto cleanupfail;
    }

    size_t strindex;
    if (elf_getshdrstrndx(ar_e, &strindex)) {
        goto cleanupfail;
    }
    Elf_Scn *section = NULL;
    while (section = elf_nextscn(ar_e, section)) {
        GElf_Shdr shdr;
        gelf_getshdr(section, &shdr);

        char *section_name = elf_strptr(ar_e, strindex, shdr.sh_name);
        if (!section_name) {
            continue;
        }
        if (shdr.sh_type == SHT_REL && !strstr(section_name,".debug")) {
            Elf_Data *data = NULL;
            data = elf_getdata(section, data);
            size_t i;
            for (i=0;i<(shdr.sh_size/shdr.sh_entsize);i+=1) {
                GElf_Rel rel;
                if (!gelf_getrel(data, i, &rel)) printf("Cannot\n");
                uint32_t offset = htole32(rel.r_offset);

                if (ELF32_R_TYPE(rel.r_info) == R_ARM_ABS32 &&
                    rel.r_offset != 0x40 /* hacky fix for __got_size bug */ ) {
                    array_add(array_data, offset);
                    printf("%s offset: %lx info: %d\n", section_name, rel.r_offset, ELF32_R_TYPE(rel.r_info));

                }
            }
        }
    }

    uint32_t * array = array_data->ptr;
    *count = array_data->curr_index;
    array_free(array_data);
    return array;

    cleanupfail:
    array_free_all(array_data);
    return NULL;
}