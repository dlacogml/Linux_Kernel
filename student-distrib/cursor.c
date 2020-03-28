#include "cursor.h"


// void init_cursor(){

// }


void update_cursor(uint16_t pos){
    outb(0x0f, 0x3d4);
    outb((uint8_t)(pos & 0xff), 0x3d5);
    outb(0x0E, 0x3d4);
    outb((uint8_t)((pos >> 8) & 0xff), 0x3d5);
}
