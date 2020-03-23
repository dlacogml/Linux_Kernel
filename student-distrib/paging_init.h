
#ifndef PAGING_INIT_H
#define PAGING_INIT_H


/* 
 * these two functions sets the correct bits in the control
 * registers to enable paging
 */
void loadPageDirectory(unsigned int*);
void enablePaging();


#endif 
