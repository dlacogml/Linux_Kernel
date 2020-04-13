#include "syscallhandlers.h"
#include "filesystem.h"
#include "paging.h"
#include "x86_desc.h"

/* global variable for storing parent pcb */
pcb_t* parent = NULL;

/* array for open processes, initalize all processes to free */
int32_t pid_array[MAX_PROCESSES] = {PID_FREE, PID_FREE, PID_FREE, PID_FREE, PID_FREE, PID_FREE};

/* holds status for execute */
int32_t global_status;

/*int32_t halt (uint8_t status)*/
/*interface: halt the process by switching back to the previous process's stack, first we extract the */
/*           the current pcb pointer and close all the files within the current pcb. Then we make sure */
/*           that halting in the base shell will only restart the current process. Then we free the pid*/
/*           for current process then link back to the parent program*/
/*input: status of the process */
/*output: nothing*/
/*return value: will never reaches to the part that returns the value*/
/*side effect: halt the current process and perform a contet switch back to the parent process*/
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

    /* if exit on base shell, restart shell */
    if (!pcb_pointer->is_haltable){
        pid_array[pcb_pointer->pid] = PID_FREE;
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
    /* set the correct status to return from the parent process*/
    if (status ==  EXCEPTION_STATUS){
        global_status = EXCEPTION_CODE;
    } else {
        global_status = (int32_t) status;
    }

    /* free pid */
    pid_array[pcb_pointer->pid] = PID_FREE;

    /* link back to parent program */
    parent_esp = pcb_pointer->esp;
    parent_ebp = pcb_pointer->ebp;
    parent = pcb_pointer->parent_pcb;

    /*reset esp and ebp to return to the parent process*/
    asm volatile("movl %0, %%esp            \n\
                  movl %1, %%ebp            \n\
                  jmp halt_return           \n\
                  "
                  :
                  :"r"(parent_esp), "r"(parent_ebp)
                  : "esp", "ebp"
                  );
    /* never reach here*/
    return 0;
}
/*int32_t execute (uint8_t status)*/
/*interface: First, read the command and find if it exists a corresponding file that's executable*/
/*           after set up the page for user, we copy the file content into the corresponding page, */
/*           next, we make an pcb object on the correct address on the kernel page, and at the end we*/
/*           push in the args for context switch.*/
/*input: command to be excuted*/
/*output: none*/
/*return value: -1 -- invalid input, or reach maximum #'s process*/
/*              256 -- when a program is terminated by exception*/ 
/*              anything in between -- defined by user program */
/*side effect: excute the command, context switch to the user stack*/
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
    /* check if file existed*/
    if (read_dentry_by_name(command, &dentry) == -1)
    {
        return -1;
    }

    /* check if the file is of type file*/
    if (dentry.filetype != FILE_CODE)
    {
        return -1;
    }

    /* filesize */
    filesize = inodes[dentry.inode_num].length;

    /* check if the file holds valid context*/
    if (read_data(dentry.inode_num, 0, (uint8_t*)buf, NUM_METADATA_BITS) == 0)
    {
        return -1;
    }

    /* check if the file's magic number is excutable */
    if (strncmp(buf, magic_number, NUM_MAGIC_BITS) != 0)
    {
        return -1;
    }

    /* check for open process */
    for (i = 0; i < MAX_PROCESSES; i++)
    {
        if (pid_array[i] == PID_FREE)
        {
            pid_array[i] = PID_TAKEN;
            break;
        }
    }

    /* check if we reach the maximum #'s of processes allows*/
    if (i == MAX_PROCESSES)
    {
        return -1;
    }

    /* valid executable, begin executing */

    /* extract entry address from metadata bytes 24-27 */
    uint8_t entry_addr[4] = {buf[24], buf[25], buf[26], buf[27]};

    /* set up paging: maps virtual addr to new 4MB physical page, set up page directory entry */
    setup_program_page(i);

    /* copy entire executable into virtual memory starting at virtual addr 0x08048000 */
    read_data((uint32_t)dentry.inode_num, (uint32_t)0, (uint8_t*)mem_start, (uint32_t)filesize);

    /* create pcb */
    pcb = (pcb_t*) (KERNEL_BOTTOM - (i + 1) * _8KB);

    /* fill in pcb */
    pcb->pid = i;
    if (i == 0)
    {
        pcb->is_haltable = 0;
    } else 
    {
        pcb->is_haltable = 1;
    }
    /* update parent */
    pcb->parent_pcb = parent;
    parent = (pcb_t*) (KERNEL_BOTTOM - (i + 1) * _8KB);

    /* fill in filename */
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
    entry_point = *((uint32_t*) entry_addr);

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

/*int32_t read (int32_t fd, void* buf, int32_t nbytes)*/
/*interface: First, check for validity of fd number, and sti to allow interrupt. then set the proper esp*/
/*           and direct to the read operation corresponding to the fd number*/
/*input: fd -- file descriptor number*/
/*       buf -- buffer to be written*/
/*       nbytes -- number of bytes to be written*/
/*output: none*/
/*return value: -1 -- failure*/
/*              whatever the directed read function returns -- success*/
/*side effect: direct to a read function*/
int32_t read (int32_t fd, void* buf, int32_t nbytes){
    /* check for valid fd */
    if (fd < 0 || fd >= NUM_FD){
        return -1;
    }
    sti();

    /* extract pcb from esp */
    register int32_t esp asm("esp");
    uint32_t mask = PCB_MASK;
    pcb_t* pcb_pointer = (pcb_t*)(esp & mask);

    /* check if file is open or not */
    if (pcb_pointer->fdarray[fd].flags == FILE_OPEN){
        return -1;
    }
    return (*pcb_pointer->fdarray[fd].f_ops_pointer->read)(fd, buf, nbytes);
}

/*int32_t write (int32_t fd, const void* buf, int32_t nbytes)*/
/*interface: First, check for validity of fd number, and check if the file can be written then set the proper esp*/
/*           and direct to the write operation corresponding to the fd number*/
/*input: fd -- file descriptor number*/
/*       buf -- buffer to be written*/
/*       nbytes -- number of bytes to be written*/
/*output: written on the screen or change value in a file*/
/*return value: -1 -- failure*/
/*              whatever the directed read function returns -- success*/
/*side effect: direct to a write function*/
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    /* check for valid fd */
    if (fd < 0 || fd >= NUM_FD){
        return -1;
    }

    /* extract pcb from esp */
    register int32_t esp asm("esp");
    uint32_t mask = PCB_MASK;
    pcb_t* pcb_pointer = (pcb_t*)(esp & mask);
    
    /* check if file is open or not */
    if (pcb_pointer->fdarray[fd].flags == FILE_OPEN){
        return -1;
    }
    return (*pcb_pointer->fdarray[fd].f_ops_pointer->write)(fd, buf, nbytes);
}
/*int32_t open (const uint8_t* filename)*/
/*interface: First, check if a file can be opened, then find the file's matadata in the filesystem*/
/*           then fill in the pcb with the corresponding value*/
/*input: filename -- name of the file to be open*/
/*output: none*/
/*return value: -1 -- failure*/
/*              fd -- success*/
/*side effect: opened a file and fill in pcb with respect to their file type*/
int32_t open (const uint8_t* filename){
    int32_t i;
    int32_t open_flag = 0;
    /* extract pcb from esp */
    register int32_t esp asm("esp");
    uint32_t mask = PCB_MASK;
    pcb_t* pcb_pointer = (pcb_t*)(esp & mask);

    /* check for open fds */
    for (i = 0; i < NUM_FD; i++){
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

    /* fill in fdarray based on file type */
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

/*close (int32_t fd)*/
/*interface: First, check if a file can be closed and its fd's validity*/
/*           then free the entry of the file in the fdarray in the corresponding process*/
/*input: fd -- fd number of the file*/
/*output: none*/
/*return value: -1 -- failure*/
/*              0 -- success*/
/*side effect: free a file from fd array*/

int32_t close (int32_t fd){
    /* extract pcb from esp */
    register int32_t esp asm("esp");
    uint32_t mask = PCB_MASK;
    pcb_t* pcb_pointer = (pcb_t*)(esp & mask);

    /* check if file open or not */
    if (pcb_pointer->fdarray[fd].flags == FILE_OPEN){
        return -1;
    }

    /* check for valid fd */
    if (fd < FIRST_NON_STD || fd >= NUM_FD){
        return -1;
    }

    /* reset fd */
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


