/*
 * keyboard.c - functions to interact with the keyboard
 *
 */

#include "lib.h"
#include "keyboard.h"
#include "i8259.h"
#define MAP_SIZE 128     //the size of keyboard map
#define KEYBOARD_IRQ 1   //the irq num for keybaord 
#define CAPS_IDX 58      //index of caps
#define CAPS_OFFSET 32  //offset to make print out the caps char

static volatile unsigned CAPS_PRESSED = 0;
static volatile unsigned SHIFT_PRESSED = 0;
static volatile unsigned CONTROL_PRESSED = 0;
static volatile unsigned ALT_PRESSED = 0;
static volatile unsigned TAB_PRESSED = 0;

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
  shift_map is a scancode table used to layout a shifted standard US keyboard
*/
unsigned char shift_map[MAP_SIZE] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',     /* 9 */
  '(', ')', '_', '+', '\b',     /* Backspace */
  '\t',                 /* Tab */
  'q', 'w', 'e', 'r',   /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0,                  /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':',     /* 39 */
 '\"', '~',   0,                /* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',                    /* 49 */
  'm', '<', '>', '?'
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
void handler33() 
{
    int8_t key = inb(KEYBOARD_DATA_REG); //retrieves keycode
    printf("%d",key);

    // //when left shift and right shift is released
    // if((key == -86 || key == -74) && SHIFT_PRESSED == 1) //scancodes of releasing left shift and right shift are -86, -74
    //   SHIFT_PRESSED = 0; //set pressed to 0
    // //when tab is pressed 
    // if()

    // if (key > 0) 
    // { 
    //   //when left shift and right is pressed 
    //   if(key == 42 || key == 54) //scancodes of pressing left shift and right shift are 42, 54
    //     SHIFT_PRESSED = 1;
    //   int ascii_val = keyboard_map[(int)key]; //ascii value of the key we just obtained
    //   int sh_ascii_val = shift_map[(int)key]; //shift ascii value of the key we just obtained
    //   //when capslock is pressed 
    //   if(key == CAPS_IDX)
    //   {
    //     CAPS_PRESSED ^= 1; //caps lock is enabled
    //   }
    //   //if the key thats a character
    //   if(ascii_val >= 97 && ascii_val <= 122) //the range of lower case ascii letters are from 97 to 122
    //   {
    //     //if only one of those is pressed 
    //     if(CAPS_PRESSED ^ SHIFT_PRESSED) 
    //     {
    //       putc(ascii_val - CAPS_OFFSET); //print the caps char of the key, and when a shift is not pressed 
    //     }
    //     else //when both are pressed or neither is pressed
    //     {
    //       putc(ascii_val);
    //     }
    //   }
    //   else if(ascii_val != 0) //if the input is not a character 
    //   {
    //     if(SHIFT_PRESSED)
    //       putc(sh_ascii_val);
    //     else
    //       putc(ascii_val);
    //   }
    // }
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


