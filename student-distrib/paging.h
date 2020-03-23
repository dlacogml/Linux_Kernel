#ifndef PAGING_H
#define PAGING_H

#ifndef ASM

#include "lib.h"
#include "paging_init.h"


uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));
extern void init_paging();


#endif
#endif 

