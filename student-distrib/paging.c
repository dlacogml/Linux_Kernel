/*
 * paging.c - functions to interact with paging
 *
 */
#include "paging.h"
#define NOT_PRESENT 0x02

//void init_paging
//interface: first set every entry of page directory to be not_present then set every entry of first page  
//           table to be not_present, then we manually set up the kernal page and the video memory page 
//           and load the page directory and enable paging.
//intput: none
//output: none
//side effects: initialize paging and set up page directory and table
void init_paging()
{
    int i;
    //initialize page directory
    for(i = 0; i < DIRECTORY_SIZE; i++)
    {
        page_directory[i] = NOT_PRESENT;
    }
    //initialize page table
    for (i = 0; i < TABLE_SIZE; i++)
    {
        first_page_table[i] = NOT_PRESENT;  
    }
    // add video memory
    //add the pages for video memory
    //shift 12 to shift VIDEO 3 left of the hex so it's aligned in the base address section
    //OR with 3 to enable P and R/W
    first_page_table[VIDEO/ALIGNED_SIZE] =  VIDEO | 3;

    //add the page
    //OR with 3 to enable P and R/W
    page_directory[0] = ((unsigned int)first_page_table) | 3; //set the first page with video memory
    //OR with 0x193 to enable P, R/W, PS, PCD, G 
    page_directory[1] = (KERNEL_ADDR | 0x193);//set the second page for kernel memory 

    //enable paging
    loadPageDirectory((unsigned int*)page_directory);
    enablePaging();

}

void setup_program_page(int pid){
    page_directory[31] = ((0x800000 + pid * 0x400000) | 0x193);
    // flush tlb
    asm volatile ("movl %cr3, %eax  \n\
                   movl %eax, %cr3  \n\
                   ");
}

