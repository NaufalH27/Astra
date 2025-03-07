#include "uintset.h"

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

int create_uintset(u_intset *s_buffer) {
    if (INIT_SIZE <= 0) {
        printf("negative value when create u_intset\n");
        return -1;
    }

    size_t byte_size = BITNSLOTS(INIT_SIZE);
    unsigned char *bitset = (unsigned char *)malloc(byte_size);
    if (bitset == NULL) {
        printf("cant allocate memory for creating u_intset\n");
        return -1;
    }
    
    memset(bitset, 0, byte_size);
    s_buffer->size = byte_size * CHAR_BIT;
    s_buffer->bitset = bitset; 
    return 0;
}

int resize_bitset(unsigned char *bitset_buffer, size_t new_size) {
    unsigned char *new_bitset = (unsigned char *)malloc(new_size);
    if (new_bitset == NULL) {
        printf("cant allocate memory for resize u_intset\n");
        return -1;
    }
    unsigned char *temp = bitset_buffer;
    bitset_buffer = new_bitset;
    free(temp);
    return 0;
}

int add_to_uintset(u_intset *s_buffer, size_t num) {
    if (num > s_buffer->size) {
        size_t new_size = BITNSLOTS(num);
        if((resize_bitset(s_buffer->bitset, new_size)) == -1){
            return -1;
        }
        s_buffer->size = new_size * CHAR_BIT;
    }
    BITSET(s_buffer->bitset, num);
    return 0;
}

void delete_from_uintset(u_intset *s_buffer, size_t num) {
    if (num > s_buffer->size) {
        return;
    }
    BITCLEAR(s_buffer->bitset, num);
}

int is_exist_in_uintset(u_intset *s_buffer, size_t num) {
    if (num > s_buffer->size) {
        return 1;
    }

    if(BITTEST(s_buffer->bitset, num) != 0) {
        return 0;
    } else {
        return 1;
    }
}


