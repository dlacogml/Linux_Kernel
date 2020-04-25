#ifndef PIT_H
#define PIT_H

#include "i8259.h"
#include "scheduling.h"

void handler32();
void init_pit(int hz);


#endif






