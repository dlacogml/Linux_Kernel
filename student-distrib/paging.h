#ifndef PAGING_H
#define PAGING_H

#include "lib.h"

extern void init_paging();
extern void loadPageDirectory(unsigned int*);
extern void enablePaging();

#endif 

