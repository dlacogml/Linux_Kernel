#include "scheduling.h"
#include "keyboard.h"

void schedule(){
    // save esp and ebp of previous process
    cli();

    asm volatile("movl %%esp, %0            \n\
                movl %%ebp, %1            \n\
                "
                :"=r"(t_s[cur_ter].esp), "=r"(t_s[cur_ter].ebp)
                :
                :"memory"
                );

    cur_ter = (cur_ter + 1) % 3;
    // set up program page
    // set tss values
    if (t_s[cur_ter].term_started == 1){
        setup_program_page(t_s[cur_ter].current_running_pid);
        tss.esp0 = _8MB - t_s[cur_ter].current_running_pid * _8KB - END_OFFSET;
        tss.ss0 = KERNEL_DS;
        asm volatile("movl %0, %%esp           \n\
                  movl %1, %%ebp            \n\
                  "
                  :
                  :"r"(t_s[cur_ter].esp), "r"(t_s[cur_ter].ebp)
                  :"memory"
                  );
    }

    if (t_s[cur_ter].term_started == 0){
        int pos;
        t_s[cur_ter].term_started = 1;

        sti();
        
        execute((uint8_t*)"shell");
    }
    sti();



    // restore esp and ebp of next process
}




