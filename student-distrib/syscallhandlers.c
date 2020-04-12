#include "syscallhandlers.h"
#include "filesystem.h"
#include "paging.h"
#include "x86_desc.h"

/* global variable for storing parent pcb */
pcb_t* parent = NULL;

/* array for open processes */
int32_t pid_array[MAX_PROCESSES] = {0, 0, 0, 0, 0, 0};

/* holds status for execute */
int32_t global_status;

int32_t halt (uint8_t status){
    /* declare local variables */
    register int32_t esp asm("esp");
    uint32_t mask = PCB_MASK;
    pcb_t* parent_pcb;
    uint32_t parent_pid, parent_esp, parent_ebp;
    int i;

    /* extract pcb pointer from esp */
    pcb_t* pcb_pointer = (pcb_t*)(esp & mask);


    /* close all fds */
    for (i = FIRST_NON_STD; i < NUM_FD; i++){
        pcb_pointer->fdarray[i].f_ops_pointer = 0;
        pcb_pointer->fdarray[i].inode = 0;
        pcb_pointer->fdarray[i].file_pos = 0;
        pcb_pointer->fdarray[i].flags = FILE_OPEN;
    }
    for (i = FIRST_NON_STD; i < NUM_FD; i++){
        if(pcb_pointer->fdarray[i].flags == FILE_CLOSED) {
            pcb_pointer->fdarray[i].f_ops_pointer->close(i);
        }
    }

    /* if exit on base shell, restart shell */
    if (!pcb_pointer->is_haltable){
        pid_array[pcb_pointer->pid] = 0;
        execute((uint8_t*)"shell");
    }

    /* grab parent pcb and pid */
    parent_pcb = (pcb_t*)pcb_pointer->parent_pcb;
    parent_pid = parent_pcb->pid;

    /* set values in tss */
    tss.esp0 = _8MB - parent_pid * _8KB - END_OFFSET;
    tss.ss0 = KERNEL_DS;

    /* load program page of parent */
    setup_program_page(parent_pid); 

    /* update global status */
    // if (status ==  EXCEPTION_STATUS){
    //     global_status = EXCEPTION_CODE;
    // } else {
    global_status = (int32_t) status;
    // }

    /* free pid */
    pid_array[pcb_pointer->pid] = PID_FREE;

    /* link back to parent program */
    parent_esp = pcb_pointer->esp;
    parent_ebp = pcb_pointer->ebp;
    parent = pcb_pointer->parent_pcb;
    asm volatile("movl %0, %%esp            \n\
                  movl %1, %%ebp            \n\
                  jmp halt_return           \n\
                  "
                  :
                  :"r"(parent_esp), "r"(parent_ebp)
                  : "esp", "ebp"
                  );

    return 0;
}

int32_t execute (const uint8_t* command){
    /* declare local variables */
    dentry_t dentry;
    int32_t filesize, i;
    int8_t magic_number[4] = {0x7f, 0x45, 0x4c, 0x46}; // magic number to check for executable
    int8_t buf[NUM_METADATA_BITS];
    uint32_t user_ds;
    uint32_t user_esp;
    uint32_t user_cs;
    uint32_t entry_point;
    pcb_t* pcb;
    uint32_t v_addr = V_ADDR_START;
    uint32_t mem_start = PROGRAM_START;

    /* check for valid executable */
    if (read_dentry_by_name(command, &dentry) == -1){
        return -1;
    }
    if (dentry.filetype != FILE_CODE){
        return -1;
    }
    filesize = inodes[dentry.inode_num].length;
    if (read_data(dentry.inode_num, 0, (uint8_t*)buf, NUM_METADATA_BITS) == 0){
        return -1;
    }
    if (strncmp(buf, magic_number, NUM_MAGIC_BITS) != 0){
        return -1;
    }

    /* check for open process */
    for (i = 0; i < MAX_PROCESSES; i++){
        if (pid_array[i] == PID_FREE){
            pid_array[i] = PID_TAKEN;
            break;
        }
    }
    if (i == MAX_PROCESSES){
        return -1;
    }

    /* extract virtual address from metadata */
    uint8_t virtual_addr[4] = {buf[24], buf[25], buf[26], buf[27]};

    /* set up paging: maps virtual addr to new 4MB physical page, set up page directory entry */
    setup_program_page(i);

    /* copy entire executable into virtual memory starting at virtual addr 0x08048000 */
    read_data((uint32_t)dentry.inode_num, (uint32_t)0, (uint8_t*)mem_start, (uint32_t)filesize);
    /* create pcb/open fds */
    pcb = (pcb_t*) (KERNEL_BOTTOM - (i + 1) * _8KB);
    /* fill in pcb */
    pcb->pid = i;
    if (i == 0){
        pcb->is_haltable = 0;
    } else {
        pcb->is_haltable = 1;
    }
    /* update parent */
    pcb->parent_pcb = parent;
    parent = (pcb_t*) (KERNEL_BOTTOM - (i + 1) * _8KB);
    strncpy((int8_t*)pcb->filename, (int8_t*)command, strlen((int8_t*)command));

    /* fill in stdin */
    pcb->fdarray[0].f_ops_pointer = &stdin_op_table;
    pcb->fdarray[0].f_ops_pointer->read = &terminal_read;
    pcb->fdarray[0].f_ops_pointer->write = &stdin_write;
    pcb->fdarray[0].f_ops_pointer->open = &terminal_open;
    pcb->fdarray[0].f_ops_pointer->close = &terminal_close;
    pcb->fdarray[0].file_pos = 0;
    pcb->fdarray[0].inode = 0;
    pcb->fdarray[0].flags = FILE_CLOSED;

    /* fill in stdout */
    pcb->fdarray[1].f_ops_pointer = &stdout_op_table;
    pcb->fdarray[1].f_ops_pointer->write = &terminal_write;
    pcb->fdarray[1].f_ops_pointer->read = &stdout_read;
    pcb->fdarray[1].f_ops_pointer->open = &terminal_open;
    pcb->fdarray[1].f_ops_pointer->close = &terminal_close;
    pcb->fdarray[1].file_pos = 0;
    pcb->fdarray[1].inode = 0;
    pcb->fdarray[1].flags = FILE_CLOSED;


    /* get current value of esp and ebp */
    asm volatile (" movl %%esp, %0      \n\
                    movl %%ebp, %1      \n\
                  "
                  : "=r"(pcb->esp), "=r"(pcb->ebp)
                  :
                  : "esp", "ebp"
    );

    /* prepare for context switch */
    tss.esp0 = _8MB - i * _8KB - END_OFFSET;
    tss.ss0 = KERNEL_DS;
    user_ds = USER_DS;
    user_esp = v_addr + _4MB - END_OFFSET;
    user_cs = USER_CS;
    entry_point = *((uint32_t*) virtual_addr);

    /* jump to entry point (entry_point) */
    asm volatile (" push %0             \n\
                    push %1             \n\
                    pushfl              \n\
                    popl %%eax          \n\
                    orl $0x200, %%eax  \n\
                    pushl %%eax         \n\
                    push %2             \n\
                    push %3             \n\
                    iret                \n\
                    "
                    :
                    :"r"(user_ds), "r"(user_esp), "r"(user_cs), "r"(entry_point)
                    :"eax"
                    );
    /* halt return */
    asm volatile( "halt_return:        \n\
                   "
    );
    return global_status;
}

int32_t read (int32_t fd, void* buf, int32_t nbytes){
    /* check for valid fd */
    if (fd < 0 || fd >= NUM_FD){
        printf("read invalid fd: %d\n", fd);
        return -1;
    }
    sti();

    /* extract pcb from esp */
    register int32_t esp asm("esp");
    uint32_t mask = PCB_MASK;
    pcb_t* pcb_pointer = (pcb_t*)(esp & mask);

    /* check if file is open or not */
    if (pcb_pointer->fdarray[fd].flags == FILE_OPEN){
        printf("read file is not open\n");
        return -1;
    }
    return (*pcb_pointer->fdarray[fd].f_ops_pointer->read)(fd, buf, nbytes);
}

int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    /* check for valid fd */
    if (fd < 0 || fd >= NUM_FD){
        printf("write invalid fd: %d\n", fd);
        return -1;
    }
    sti();

    /* extract pcb from esp */
    register int32_t esp asm("esp");
    uint32_t mask = PCB_MASK;
    pcb_t* pcb_pointer = (pcb_t*)(esp & mask);
    
    /* check if file is open or not */
    if (pcb_pointer->fdarray[fd].flags == FILE_OPEN){
        printf("write file is not open\n");

        return -1;
    }

    return (*pcb_pointer->fdarray[fd].f_ops_pointer->write)(fd, buf, nbytes);
}

int32_t open (const uint8_t* filename){
    int32_t i;
    int32_t open_flag = 0;
    /* extract pcb from esp */
    register int32_t esp asm("esp");
    uint32_t mask = PCB_MASK;
    pcb_t* pcb_pointer = (pcb_t*)(esp & mask);

    /*  */
    for (i = 0; i < NUM_FD; i++){
        // printf("%d\n", i);
        if (pcb_pointer->fdarray[i].flags == FILE_OPEN){
            open_flag = 1;
            break;
        }
    }
    if (open_flag == 0 || i == 8){
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

        printf("fd: %d", fd);
        return fd;
    }
    return -1;
}

int32_t close (int32_t fd){
    register int32_t esp asm("esp");
    uint32_t mask = PCB_MASK;
    pcb_t* pcb_pointer = (pcb_t*)(esp & mask);
    if (pcb_pointer->fdarray[fd].flags == FILE_OPEN){
        return -1;
    }
    if (fd < FIRST_NON_STD || fd >= NUM_FD){
        return -1;
    }
    pcb_pointer->fdarray[fd].f_ops_pointer = 0;
    pcb_pointer->fdarray[fd].inode = 0;
    pcb_pointer->fdarray[fd].file_pos = 0;
    pcb_pointer->fdarray[fd].flags = FILE_OPEN;
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


