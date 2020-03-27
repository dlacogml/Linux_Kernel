/*
*/
#include "terminal.h"
#define BUF_SIZE     129    //maximum number of chars 128 buffer can hold + NULL + newline 

static unsigned char NN_BUFFER[2] = {'\n', 0};
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
    if(nbytes < 2)
        return -1;
    //wait until the newline signal is triggered
    while(!NEWLINE_FLAG)
    {
        // printf("1123");
        //do nothing 
    }
    //reset the newline flag
    NEWLINE_FLAG = 0;
    if(nbytes < buf_idx+1)
    {
        /*nbytes < actual size*/
        memcpy(buf, keyboard_buffer, nbytes-2);
        memcpy(buf+nbytes-2, NN_BUFFER, 2);
        clear_buffer();
        return nbytes;
    }
    else if(nbytes == buf_idx+1)
    {
        /*perfecrt situation*/
        memcpy(buf, keyboard_buffer, nbytes);
        clear_buffer();
        return nbytes;
    }
    else if(nbytes > buf_idx+1)
    {
        /*nbytes > actual size*/
        memcpy(buf, keyboard_buffer, buf_idx+1);
        clear_buffer();
        return buf_idx+1;
    }
    return 0;
}

uint32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes)
{
    if(buf == NULL)
        return -1;
    uint32_t i;
    unsigned char * a = buf;
    for(i = 0; i < nbytes; i++)
    {
        if(a[i] == 0)
            return i;
        putc(a[i]);
    }
    return nbytes;
}


