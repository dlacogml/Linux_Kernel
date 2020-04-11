#include "syscallhandlers.h"
#include "filesystem.h"
#include "paging.h"
#include "x86_desc.h"

pcb_t* parent = NULL;
int32_t pid_array[6] = {0, 0, 0, 0, 0, 0};

int32_t halt (uint8_t status){
    return 0;
}

int32_t execute (const uint8_t* command){
    dentry_t dentry;
    if (read_dentry_by_name(command, &dentry) == -1){
        return -1;
    }
    if (dentry.filetype != FILE_CODE){
        return -1;
    }
    int filesize = inodes[dentry.inode_num].length;
    uint8_t magic_number[4] = {0x7f, 0x45, 0x4c, 0x46};
    uint8_t buf[40];
    if (read_data(dentry.inode_num, 0, buf, 40) == 0){
        return -1;
    }
    if (strncmp(buf, magic_number, 4) != 0){
        return -1;
    }
    int i;
    for (i = 0; i < 6; i++){
        if (pid_array[i] == 0){
            pid_array[i] = 1;
            break;
        }
    }
    uint8_t virtual_addr[4] = {buf[24], buf[25], buf[26], buf[27]};

    //set up paging: maps virtual addr to new 4MB physical page, set up page directory entry
    setup_program_page(i);
    uint32_t v_addr = 0x08000000;
    uint32_t mem_start = 0x08048000;

    //copy entire executable into virtual memory starting at virtual addr 0x08048000
    read_data(dentry.inode_num, 0, mem_start, filesize);
    //create pcb/open fds
    pcb_t pcb;
    // fill in pcb
    pcb.pid = i;
    if (i == 0){
        pcb.is_haltable = 0;
    } else {
        pcb.is_haltable = 1;
    }
    pcb.parent_pcb = parent;
    parent = &pcb;
    strncpy(pcb.filename, command, strlen(command));
    pcb.fdarray[0].f_ops_pointer = &terminal_op_table;
    pcb.fdarray[0].f_ops_pointer->read = &(terminal_read);
    pcb.fdarray[0].f_ops_pointer->write = &stdin_write;
    pcb.fdarray[0].f_ops_pointer->open = &terminal_open;
    pcb.fdarray[0].f_ops_pointer->close = &terminal_close;
    pcb.fdarray[0].file_pos = 0;
    pcb.fdarray[0].inode = 0;
    pcb.fdarray[0].flags = 1;

    pcb.fdarray[1].f_ops_pointer = &terminal_op_table;
    pcb.fdarray[1].f_ops_pointer->write = &(terminal_write);
    pcb.fdarray[1].f_ops_pointer->read = &(stdout_read);
    pcb.fdarray[1].f_ops_pointer->open = &terminal_open;
    pcb.fdarray[1].f_ops_pointer->close = &terminal_close;
    pcb.fdarray[1].file_pos = 0;
    pcb.fdarray[1].inode = 0;
    pcb.fdarray[1].flags = 1;
    memcpy(2 * KERNEL_ADDR - (i + 1) * 0x2000, &pcb, sizeof(pcb));
    //jump to entry point (entry_point) 

    // prepare for context switch
    uint32_t user_ds = USER_DS;
    uint32_t user_esp = v_addr + 0x3fffff - 3;
    uint32_t user_cs = USER_CS;
    tss.esp0 = 0x800000 - i * 0x2000 - 4;
    uint32_t entry_point = *((uint32_t*) virtual_addr);

    asm volatile (" push %0     \n\
                    push %1     \n\
                    pushfl     \n\
                    push %2     \n\
                    push %3     \n\
                    iret"
                    :
                    :"r"(user_ds), "r"(user_esp), "r"(user_cs), "r"(entry_point)
                    :"memory"
                    );
    return 0;
}

int32_t read (int32_t fd, void* buf, int32_t nbytes){
    register int esp asm("esp");
    uint32_t mask = 0xffffe000;
    pcb_t* pcb_pointer = esp & mask;
    return (*pcb_pointer->fdarray[fd].f_ops_pointer->read)(fd, buf, nbytes);
}

int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    register int esp asm("esp");
    uint32_t mask = 0xffffe000;
    pcb_t* pcb_pointer = esp & mask;
    return (*pcb_pointer->fdarray[fd].f_ops_pointer->write)(fd, buf, nbytes);
}

int32_t open (const uint8_t* filename){
    int32_t i;
    int32_t open_flag = 0;
    register int esp asm("esp");
    uint32_t mask = 0xffffe000;
    pcb_t* pcb_pointer = esp & mask;

    for (i = 0; i < NUM_FD; i++){
        if (pcb_pointer->fdarray[i].flags == FILE_OPEN){
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
            pcb_pointer->fdarray[fd].f_ops_pointer = &file_op_table;
            pcb_pointer->fdarray[fd].f_ops_pointer->read = &file_read;
            pcb_pointer->fdarray[fd].f_ops_pointer->write = &file_write;
            pcb_pointer->fdarray[fd].f_ops_pointer->open = &file_open;
            pcb_pointer->fdarray[fd].f_ops_pointer->close = &file_close;
            pcb_pointer->fdarray[fd].inode = dentry.inode_num;
            pcb_pointer->fdarray[fd].file_pos = 0;
            pcb_pointer->fdarray[fd].flags = FILE_CLOSED;
        } else if (dentry.filetype == DIR_CODE){
            pcb_pointer->fdarray[fd].f_ops_pointer = &dir_op_table;
            pcb_pointer->fdarray[fd].f_ops_pointer->read = &dir_read;
            pcb_pointer->fdarray[fd].f_ops_pointer->write = &dir_write;
            pcb_pointer->fdarray[fd].f_ops_pointer->open = &dir_open;
            pcb_pointer->fdarray[fd].f_ops_pointer->close = &dir_close;
            pcb_pointer->fdarray[fd].inode = 0;
            pcb_pointer->fdarray[fd].file_pos = 0;
            pcb_pointer->fdarray[fd].flags = FILE_CLOSED;
        } else if (dentry.filetype == RTC_CODE){
            pcb_pointer->fdarray[fd].f_ops_pointer = &rtc_op_table;
            pcb_pointer->fdarray[fd].f_ops_pointer->read = &rtc_read;
            pcb_pointer->fdarray[fd].f_ops_pointer->write = &rtc_write;
            pcb_pointer->fdarray[fd].f_ops_pointer->open = &rtc_open;
            pcb_pointer->fdarray[fd].f_ops_pointer->close = &rtc_close;
            pcb_pointer->fdarray[fd].inode = 0;
            pcb_pointer->fdarray[fd].file_pos = 0;
            pcb_pointer->fdarray[fd].flags = FILE_CLOSED;
        }


        return fd;
    }
    return -1;
}

int32_t close (int32_t fd){
    register int esp asm("esp");
    uint32_t mask = 0xffffe000;
    pcb_t* pcb_pointer = esp & mask;
    if (fd <= 1 || fd >= 8){
        return -1;
    }
    pcb_pointer->fdarray[fd].flags == FILE_OPEN;
    return 0;
}

int32_t getargs (uint8_t* buf, int32_t nbytes){
    return 0;
}

int32_t vidmap (uint8_t** screen_start){
    return 0;
}

int32_t set_handler (int32_t signum, void* handler_address){
    return 0;
}

int32_t sigreturn (void){
    return 0;
}


