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
#define BUF_SIZE     128     //maximum number of chars 127 buffer can hold + line feed
#define NUM_TERMS    3
#define PAGE_SIZE    4096
#define ADDR_ALIGN      12

uint32_t cur_ter;
uint32_t disp_ter;
int32_t keyboard_flag;
int32_t currently_typed_term;

/* terminal struct */
typedef struct terminal{
    int8_t kb_buf[BUF_SIZE];
    int32_t term_started;
    int32_t screen_x;
    int32_t screen_y;
    int8_t* video_mem_buf;
    int32_t b_idx;
    int32_t r_idx;
    int32_t current_running_pid;
    int32_t base_shell_pid;
    void* parent;
    int32_t global_status;
    int32_t newline_flag;
    int32_t esp;
    int32_t ebp;
} terminal_t;

extern terminal_t t_s[NUM_TERMS];

/*function essential to keyboards*/
void clear_buffer();
void init_keyboard();

/*function essential to terminal*/
int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(int32_t fd);
int32_t terminal_read(int32_t fd, uint8_t* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t switch_terminal(int32_t terminal_number);
void init_terminal();
void different_terminal(int32_t terminal_number);
#endif
