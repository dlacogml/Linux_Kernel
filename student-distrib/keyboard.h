/*
 *
 * keyboard.h - defines used in interactions with the keyboard interrupt 
 * 
 */


#ifndef KEYBOARD_H
#define KEYBOARD_H
#define KEYBOARD_DATA_REG 0x60
#define MAP_SIZE     128     //the size of keyboard map
#define KEYBOARD_IRQ 1       //the irq num for keybaord 
#define CAPS_IDX     58      //index of caps
#define CAPS_OFFSET  32      //offset to make print out the caps char
#define BUF_SIZE     129     //maximum number of chars 127 buffer can hold + NULL + line feed
static volatile unsigned CAPS_PRESSED = 0;
static volatile unsigned SHIFT_PRESSED = 0;
static volatile unsigned CONTROL_PRESSED = 0;
static volatile unsigned ALT_PRESSED = 0;
static volatile unsigned TAB_PRESSED = 0;
static volatile unsigned buf_idx = 0;
/*global flag and buffer*/
static unsigned char keyboard_buffer[BUF_SIZE];
static unsigned char NEWLINE_FLAG = 0;

extern void clear_buffer();

/* This function initializes the keyboard */
void init_keyboard();

#endif
