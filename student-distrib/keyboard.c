/*
 * keyboard.c - functions to interact with the keyboard
 *
 */

#include "lib.h"
#include "keyboard.h"
#include "i8259.h"
#define MAP_SIZE 128 //the size of keyboard map
#define KEYBOARD_IRQ 1 //the irq num for keybaord 
/*
 * keyboard_map is a scancode table used to layout a standard US keyboard
 */
unsigned char keyboard_map[MAP_SIZE] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
  '9', '0', '-', '=', '\b',     /* Backspace */
  '\t',                 /* Tab */
  'q', 'w', 'e', 'r',   /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0,                  /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     /* 39 */
 '\'', '`',   0,                /* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',                    /* 49 */
  'm', ',', '.', '/',   0,                              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

/*
 * void handler33()
 * 
 * Description: Handler for the keyboard. Checks if the keycode
 * retrieved from the scancode is positive and its corresponding
 * scancode doesn't return 0 and if both conditions are met,
 * will putc the corresponding key onto the screen
 * 
 * No inputs nor outputs
 */
void handler33() {
    int8_t key = inb(KEYBOARD_DATA_REG); //retrieves keycode
    if (key > 0 && keyboard_map[(int)key] != 0) { //if conditions are met, putc the corresponding key
        putc(keyboard_map[(int)key]);
    }
    send_eoi(KEYBOARD_IRQ); //end of interrupt
}

/*
 * void init_keyboard()
 * 
 * Description: enables interrupt request into initialize keyboard
 * 
 * No inputs nor outputs
 */
void init_keyboard()
{
    enable_irq(KEYBOARD_IRQ);
}


