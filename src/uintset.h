#ifndef INT_SET_H
#define INT_SET_H

#include <limits.h>
#include <stddef.h>

#define INIT_SIZE 1000

#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)

// dynamic sized set for positive integer value, this struct doesnt support negative integer value
typedef struct u_intset {
    unsigned char *bitset;
    size_t size;
} u_intset;

// create integer set in heap  
// the function return status value, return -1 if there is error
int create_uintset(u_intset *s_buffer);

int add_to_uintset(u_intset *s_buffer, size_t num);
void delete_from_uintset(u_intset *s_buffer, size_t num);
int is_exist_in_uintset(u_intset *s_buffer, size_t num);

#endif 
