#ifndef PAGING_H
#define PAGING_H

#ifndef ASM

#include "lib.h"
#include "paging_init.h"
#define DIRECTORY_SIZE 1024
#define TABLE_SIZE 1024
#define ALIGNED_SIZE 4096

//allocate memories for page_directory and first page_table
uint32_t page_directory[DIRECTORY_SIZE] __attribute__((aligned(ALIGNED_SIZE)));
uint32_t first_page_table[TABLE_SIZE] __attribute__((aligned(ALIGNED_SIZE)));
//function that calls to initialize paging
extern void init_paging();


#endif
#endif 

