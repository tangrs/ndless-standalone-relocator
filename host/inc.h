#include <stdint.h>

uint32_t* elf_get_offsets(int fd, uint32_t* count);

#define bswap32(x) \
({ \
	uint32_t __x = (x); \
	((uint32_t)( \
		(((uint32_t)(__x) & (uint32_t)0x000000FFUL) << 24) | \
		(((uint32_t)(__x) & (uint32_t)0x0000FF00UL) <<  8) | \
		(((uint32_t)(__x) & (uint32_t)0x00FF0000UL) >>  8) | \
		(((uint32_t)(__x) & (uint32_t)0xFF000000UL) >> 24) )); \
})

static inline uint32_t htole32(uint32_t x) {
    union {
        uint16_t int_val;
        uint8_t  char_val[2];
    } endian;
    endian.int_val = 1;
    if (!endian.char_val[0]) return bswap32(x);
    return x;
}
