#include "paging.h"

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void init_paging()
{
    int i;
    //initialize page directory
    for(i = 0; i < 1024; i++)
    {
        page_directory[i] = 0x00000002;
    }
    //initialize page table
    for (i = 0; i < 1024; i++)
    {
        first_page_table[i] = 0x00000002;  
    }
    // add video memory
    first_page_table[VIDEO/4096] =  (VIDEO << 3) | 3;
    //add the page
    page_directory[0] = ((unsigned int)first_page_table) | 3;
    page_directory[1] = (0x40000000 | 0x83);
    //enable paging
    asm
    (

    );


}