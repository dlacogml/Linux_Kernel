/*
 * keyboard.c - functions to interact with the keyboard
 *
 */

#include "lib.h"
#include "keyboard.h"
#include "i8259.h"

/*
 * keyboard_map is a scancode table used to layout a standard US keyboard
 */
unsigned char keyboard_map[MAP_SIZE] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
  '9', '0', '-', '=', '\b',     /* Backspace */
  0,                 /* Tab */
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
  0,                 /* Tab */
  'Q', 'W', 'E', 'R',   /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', /* Enter key */
    0,                  /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',     /* 39 */
 '\"', '~',   0,                /* Left shift */
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N',                    /* 49 */
  'M', '<', '>', '?',
   0,                              /* Right shift */
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
void handler33() 
{
    int8_t key = inb(KEYBOARD_DATA_REG); //retrieves keycode
    // printf("%d",key);

    /* checking for any specific keys being released */
    //check if enter is presssed, scancode for enter is 28
    if(key == 28 || buf_idx == 126)
    {
      NEWLINE_FLAG = 1;
      // printf("111");
      // send_eoi(KEYBOARD_IRQ);
      // return;
    }
    //when left shift and right shift is released
    if((key == -86 || key == -74) && SHIFT_PRESSED == 1) //scancodes of releasing left shift and right shift are -86, -74
      SHIFT_PRESSED = 0; //set shift pressed to 0
    
    //when alt is released
    if((key == -72) && ALT_PRESSED == 1) //scancode of releasing alt is -72
      ALT_PRESSED = 0; //set alt pressed to 0
    
    //when control is released
    if((key == -99) && CONTROL_PRESSED == 1) //scancode of releasing control is -99
      CONTROL_PRESSED = 0; //set control pressed to 0

    /* printing onto screen */
    if (key > 0) 
    { 
      //when tab is pressed
      if (key == 15) {
        TAB_PRESSED = 1;
      }
      else {
        TAB_PRESSED = 0;
      }

      //when alt is pressed
      if (key == 56)
        ALT_PRESSED = 1;

      //when control is pressed
      if (key == 29)
        CONTROL_PRESSED = 1;
  
      //when left shift and right is pressed 
      if(key == 42 || key == 54) //scancodes of pressing left shift and right shift are 42, 54
        SHIFT_PRESSED = 1;
      int ascii_val = keyboard_map[(int)key]; //ascii value of the key we just obtained
      int sh_ascii_val = shift_map[(int)key]; //shift ascii value of the key we just obtained
      
      //checking if ctrl + l or ctrl + L
      if (CONTROL_PRESSED == 1 && (ascii_val == 'l' || sh_ascii_val == 'L')) 
      {
        buf_idx = 0;
        clear_buffer(); //clear th buffer 
        clear();        //clear the screen
        send_eoi(KEYBOARD_IRQ);
        return;
      }
      //check if backspace is pressed
      if (key == 14)
      {
        if(buf_idx) //check if the index is 0
        {
          keyboard_buffer[buf_idx] = 0;
          buf_idx--;
        } 
        backspace();
        send_eoi(KEYBOARD_IRQ);
        return;
      }
      //when capslock is pressed 
      if(key == CAPS_IDX)
      {
        CAPS_PRESSED ^= 1; //caps lock is enabled
      }

      //if the key thats a character
      if(ascii_val >= 97 && ascii_val <= 122) //the range of lower case ascii letters are from 97 to 122
      {
        //if only one of those is pressed 
        if(CAPS_PRESSED ^ SHIFT_PRESSED) 
        {
          putc(sh_ascii_val); //print the caps char of the key, and when a shift is not pressed 
          keyboard_buffer[buf_idx] = sh_ascii_val;
          buf_idx++;
        }
        else //when both are pressed or neither is pressed
        {
          putc(ascii_val);
          keyboard_buffer[buf_idx] = ascii_val;
          buf_idx++;
        }
      }
      else if(ascii_val != 0) //if the input is not a character 
      {
        if(SHIFT_PRESSED)
        {
          putc(sh_ascii_val);
          keyboard_buffer[buf_idx] = sh_ascii_val;
          buf_idx++;
        }
        else
        {
          putc(ascii_val);
          keyboard_buffer[buf_idx] = ascii_val;
          buf_idx++;
        }
      }
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


void clear_buffer()
{
  uint32_t i = 0;
  for (i = 0; i < 129; i++)
  {
    keyboard_buffer[i] = 0;
  }
}