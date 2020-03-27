/*
 * keyboard.c - functions to interact with the keyboard
 *
 */

#include "lib.h"
#include "keyboard.h"
#include "i8259.h"
static volatile uint8_t CAPS_PRESSED = 0;
static volatile uint8_t SHIFT_PRESSED = 0;
static volatile uint8_t CONTROL_PRESSED = 0;
static volatile uint8_t ALT_PRESSED = 0;
static volatile uint8_t TAB_PRESSED = 0;
static volatile uint8_t NEWLINE_FLAG = 0;
static uint32_t buf_idx = 0;
static uint32_t read_idx = 0; //how many times we have read the string
static uint8_t  keyboard_buffer[BUF_SIZE];
static uint8_t  NN_BUFFER[1] = {'\n'};
/*
 * keyboard_map is a scancode table used to layout a standard US keyboard
 */
uint8_t keyboard_map[MAP_SIZE] =
{
  0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',  
  0,'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,            
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 
  'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 
};
/*
  shift_map is a scancode table used to layout a shifted standard US keyboard
*/
uint8_t shift_map[MAP_SIZE] =
{
  0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',  
  0,'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,            
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|', 
  'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 
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
    /* checking for any specific keys being released */

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
      //check if enter is presssed, scancode for enter is 28
      if(key == 28 || buf_idx == 127)
      {
        NEWLINE_FLAG = 1;
        keyboard_buffer[buf_idx] = '\n'; 
        // putc('\n');
        // scroll();
        send_eoi(KEYBOARD_IRQ);
        return;
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
      /* when capslock is pressed */
      if(key == 58)
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
  for (i = 0; i < 128; i++)
  {
    keyboard_buffer[i] = 0;
  }
  buf_idx = 0;
  read_idx = 0;
}

uint32_t terminal_open(const uint8_t* filename)
{
    //do nothing
    return 0;
}
uint32_t terminal_close(int32_t fd)
{
    //do nothing
    return 0;
}
uint32_t terminal_read(int32_t fd, void* buf, int32_t nbytes)
{
    if(nbytes < 1 || buf == NULL)
        return -1;
    //wait until the newline signal is triggered
    while(!NEWLINE_FLAG);
    //reset the newline flag
    cli();
    NEWLINE_FLAG = 0;
    // if(nbytes < buf_idx+1)
    // {
    //     /*nbytes < actual size*/
    //     memcpy(buf, keyboard_buffer, nbytes);
    //     // clear_buffer();
    //     sti();
    //     return nbytes;
    // }
    // else if(nbytes == buf_idx+1)
    // {
    //     /*perfect situation*/
    //     memcpy(buf, keyboard_buffer, nbytes);
    //     clear_buffer();
    //     sti();
    //     return nbytes;
    // }
    // else if(nbytes > buf_idx+1)
    // {
    //     /*nbytes > actual size*/
    //     memcpy(buf, keyboard_buffer, buf_idx+1);
    //     clear_buffer();
    //     sti();
    //     return buf_idx+1;
    // }
    uint32_t i;
    uint32_t count = 0;
    uint8_t *buffer = (uint8_t *)buf;
    for(i = 0; i < nbytes && keyboard_buffer[i] != 0; i++)
    {
      buffer[i] = keyboard_buffer[read_idx * nbytes+i];
      count++;
      if(buffer[i] == '\n')
      {
        clear_buffer();
        sti();
        return count;
      }
    }
    read_idx++;
    //same 
    sti();
    return count;
}

uint32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes)
{
    if(buf == NULL)
        return -1;
    uint32_t i;
    uint8_t * a = (uint8_t*) buf;
    for(i = 0; i < nbytes; i++)
    {
        if(a[i] == 0)
            return i;
        putc(a[i]);
    }
    return nbytes;
}
