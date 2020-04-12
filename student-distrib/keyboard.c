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
static uint32_t buf_idx = 0;                  //current index of the keyboard buffer
static uint32_t read_idx = 0;                 //how many times we have read the string
static uint8_t  keyboard_buffer[BUF_SIZE];    //buffer for the keyboard string
/*
 * keyboard_map is a scancode table used to layout a standard US keyboard
 */
uint8_t keyboard_map[MAP_SIZE] =
{
  0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',  
  ' ','q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,            
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 
  'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',	
};
/*
  shift_map is a scancode table used to layout a shifted standard US keyboard
*/
uint8_t shift_map[MAP_SIZE] =
{
  0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',  
  ' ','Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,            
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|', 
  'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ',
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
    /* checking for any functionality keys being released or pressed*/

    /*when left shift and right shift is released*/
    if((key == -86 || key == -74) && SHIFT_PRESSED == 1) //scancodes of releasing left shift and right shift are -86, -74
      SHIFT_PRESSED = 0; //set shift pressed to 0
    
    /*when alt is released*/
    if((key == -72) && ALT_PRESSED == 1) //scancode of releasing alt is -72
      ALT_PRESSED = 0; //set alt pressed to 0
    
    /*when control is released*/
    if((key == -99) && CONTROL_PRESSED == 1) //scancode of releasing control is -99
      CONTROL_PRESSED = 0; //set control pressed to 0

    /*check if enter is presssed, scancode for enter is 28*/
    if(key == 28)
    {
      NEWLINE_FLAG = 1; //set the newline flag to be 1
      keyboard_buffer[buf_idx] = '\n'; //append newline at the end
      putc('\n');
      send_eoi(KEYBOARD_IRQ);
      return;
    }

    /*when tab is pressed*/
    if (key == 15) 
    {
      TAB_PRESSED = 1;
    }
    else 
    {
      TAB_PRESSED = 0;
    }

    /*when alt is pressed*/
    if (key == 56)
      ALT_PRESSED = 1;

    /*when control is pressed*/
    if (key == 29)
      CONTROL_PRESSED = 1;

    /*when left shift and right is pressed*/
    if(key == 42 || key == 54) //scancodes of pressing left shift and right shift are 42, 54
      SHIFT_PRESSED = 1;

    /*check if backspace is pressed*/
    if (key == 14)
    {
      if(buf_idx) //check if the index is 0
      {
        keyboard_buffer[buf_idx] = 0;
        buf_idx--;
        backspace();
      }
      send_eoi(KEYBOARD_IRQ);
      return;
    }
    /* printing onto screen */
    if (key > 0) 
    { 
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
      /* when capslock is pressed */
      if(key == 58)
      {
        CAPS_PRESSED ^= 1; //caps lock is enabled
      }

      //if the key thats a character
      if(ascii_val >= 97 && ascii_val <= 122 && buf_idx < 127) //the range of lower case ascii letters are from 97 to 122
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
      else if(ascii_val != 0 && buf_idx < 127) //if the input is not a character 
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
/*
 * void clear_buffer()
 * interface: clear the key_board buffer and reset the buffer index and read index
 * input: none
 * output: none
 * return value: 0
 * side effects: clear the buff index
 */
void clear_buffer()
{
  uint32_t i = 0;
  for (i = 0; i < BUF_SIZE; i++)
  {
    keyboard_buffer[i] = 0; //clearing the buffer
  }
  /* reseting the index */
  buf_idx = 0;
  read_idx = 0;
}
/*
 * void terminal_open(const uint8_t* filename)
 * interface: clear the keyboad buffer
 * input: filename
 * output: one
 * return value: 0
 * side effects: nothing
 */
int32_t terminal_open(const uint8_t* filename)
{
  clear_buffer();
  return 0;  //do nothing
}
/*
 * void terminal_open(int32_t fd)
 * interface: do nothing
 * input: fd
 * output: none
 * return value: 0
 * side effects: nothing
 */
int32_t terminal_close(int32_t fd)
{
    return 0; //do nothing
}
/*
 * void terminal_read(int32_t fd, void* buf, int32_t nbytes)
 * interface: wait until enter is pressed then read the the number of bytes from key board buffer into the buf
 * input: fd : not used
 *        buf: user-level buffer to copy to
 *        nbytes: number of bytes to copy to
 * output: none
 * return value: success - number of bytes read
 *               fail    - -1
 * side effects: read the the number of bytes into the buf
 */
int32_t terminal_read(int32_t fd, uint8_t* buf, int32_t nbytes)
{
    if(nbytes < 1 || buf == NULL)
        return -1;
    //wait until the newline signal is triggered
    while(!NEWLINE_FLAG);
    // cli();
    uint32_t i; //loop counter
    uint32_t count = 0; //number of bytes read
    uint8_t *buffer = (uint8_t *)buf; //cast the buffer
    for(i = 0; i < nbytes && keyboard_buffer[i] != 0; i++)
    {
      buffer[i] = keyboard_buffer[read_idx * nbytes+i]; //copy key board buffer to buf
      count++; //increment the count every time a byte is read into buf
      /*when we reach the end*/
      if(buffer[i] == '\n')
      {
        clear_buffer();
        NEWLINE_FLAG = 0; //reset the NEW_LINE FLAG
        buffer[i] = '\0';
        // sti();
        return count;
      }
    }
    if(nbytes >= strlen((int8_t*)buf))
      NEWLINE_FLAG = 0;
    //increment the number of times a keyboard string has being read
    read_idx++;
    // sti();
    return count;
}
/*
 * void terminal_write(int32_t fd, const void* buf, int32_t nbytes)
 * interface: write the the number of bytes from input buffer on screen
 * input: fd : not used
 *        buf: user-level buffer to write
 *        nbytes: number of bytes to write
 * output: output buf on the screen
 * return value: success - number of bytes write
 *               fail    - -1
 * side effects: write the number of bytes from the buffer on screen
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes)
{
    /*check if buf is NULL*/
    if(buf == NULL)
        return -1;
    uint32_t i; //loop counter
    uint8_t * a = (uint8_t*) buf; //cast the void ptr
    for(i = 0; i < nbytes; i++)
    {
        /* when we have reach the end of the buf*/
        if(a[i] == '\n' && a[i] != 0)
        {
          // clear_buffer();   //clear the keyboard buffer after we call write
          // NEWLINE_FLAG = 0; //reset the NEW_LINE FLAG
          putc(a[i]); //output char on screen
          return i+1;
        }
        putc(a[i]); //output char on screen
    }
    return nbytes;
}
