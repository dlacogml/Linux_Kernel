/*
 *
 * keyboard.h - defines used in interactions with the keyboard interrupt 
 * 
 */


#ifndef KEYBOARD_H
#define KEYBOARD_H
/* defined constant */
#define KEYBOARD_DATA_REG 0x60
#define MAP_SIZE     128     //the size of keyboard map
#define KEYBOARD_IRQ 1       //the irq num for keybaord 
#define BUF_SIZE     1024     //maximum number of chars 127 buffer can hold + line feed

/*function essential to keyboards*/
void clear_buffer();
void init_keyboard();

/*function essential to terminal*/
int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(int32_t fd);
int32_t terminal_read(int32_t fd, uint8_t* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

#endif
