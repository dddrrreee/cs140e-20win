#ifndef __FAT32_HELPERS_H__
#define __FAT32_HELPERS_H__
#include "fat32-data-structures.h"

static inline int is_fat32(int t) { return t == 0xb; } 

#endif
