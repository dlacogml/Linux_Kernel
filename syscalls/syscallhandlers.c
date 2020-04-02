#include "syscallhandlers.h"



int32_t halt (uint8_t status){

}

int32_t execute (const uint8_t* command){

}

int32_t read (int32_t fd, void* buf, int32_t nbytes){
    return (*fdarray[fd].f_ops_pointer->read)(fd, buf, nbytes);
}

int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    return (*fdarray[fd].f_ops_pointer->write)(fd, buf, nbytes);
}

int32_t open (const uint8_t* filename){
    int32_t i;
    int32_t open_flag = 0;
    for (i = 0; i < NUM_FD; i++){
        if (fdarray[i].flags == FILE_OPEN){
            open_flag = 1;
            break;
        }
    }
    if (open_flag == 0){
        return -1;
    }
    int32_t fd = i;
    dentry_t dentry;
    if (read_dentry_by_name(filename, &dentry) == 0){
        if (dentry.filetype == FILE_CODE){
            fdarray[fd].f_ops_pointer->read = &file_read;
            fdarray[fd].f_ops_pointer->write = &file_write;
            fdarray[fd].f_ops_pointer->open = &file_open;
            fdarray[fd].f_ops_pointer->close = &file_close;
            fdarray[fd].inode = dentry.inode_num;
            fdarray[fd].file_pos = 0;
            fdarray[fd].flags = FILE_CLOSED;
        } else if (dentry.filetype == DIR_CODE){
            fdarray[fd].f_ops_pointer->read = &dir_read;
            fdarray[fd].f_ops_pointer->write = &dir_write;
            fdarray[fd].f_ops_pointer->open = &dir_open;
            fdarray[fd].f_ops_pointer->close = &dir_close;
            fdarray[fd].inode = 0;
            fdarray[fd].file_pos = 0;
            fdarray[fd].flags = FILE_CLOSED;
        } else if (dentry.filetype == RTC_CODE){
            fdarray[fd].f_ops_pointer->read = &rtc_read;
            fdarray[fd].f_ops_pointer->write = &rtc_write;
            fdarray[fd].f_ops_pointer->open = &rtc_open;
            fdarray[fd].f_ops_pointer->close = &rtc_close;
            fdarray[fd].inode = 0;
            fdarray[fd].file_pos = 0;
            fdarray[fd].flags = FILE_CLOSED;
        }


        return fd;
    }
    return -1;
}

int32_t close (int32_t fd){
    if (fdarray[fd].flags == FILE_CLOSED){
        fdarray[fd].flags == FILE_OPEN;
    }
}

int32_t getargs (uint8_t* buf, int32_t nbytes){

}

int32_t vidmap (uint8_t** screen_start){

}

int32_t set_handler (int32_t signum, void* handler_address){

}

int32_t sigreturn (void){

}