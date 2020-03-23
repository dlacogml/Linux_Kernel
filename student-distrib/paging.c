#include "paging.h"


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
    first_page_table[VIDEO/4096] =  (VIDEO << 12) | 3;

    //add the page
    page_directory[0] = ((unsigned int)first_page_table) | 3;
    page_directory[1] = (0x400000 | 0x193);

    //enable paging
    loadPageDirectory((unsigned int)page_directory);
    enablePaging();

}

