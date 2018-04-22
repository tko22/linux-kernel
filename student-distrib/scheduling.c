#include "scheduling.h"
#include "sys.h"
#include "i8259.h"
#include "x86_desc.h"
#include "paging.h"
#include "lib.h"

volatile uint8_t n_pid;
#define TERM_VID_BUFF 0x40000000
#define EIGHTKB 0x2000      //2^13
#define EIGHTMB 0x00800000

void initalize_PIT(){
    outb(0x36, 0x43);
    outb(Thirty_HZ & 0xFF ,0x40);
    outb(Thirty_HZ >> 8 ,0x40);
    enable_irq(0); //IRQ 0 is for PIT
}

void handle_pit_interrupt(){
  	send_eoi(0);
    // handle PIT here !!, call something
    // pcb_t* curr;
    // curr = get_last_pcb();                      //get current process
    // process = curr->pid;
    // next_process(process);                      //get next process
    do_switch();                                //call function that does restructuring of the stack
}

uint8_t next_process(uint8_t process){
    int i;

    for(i = 1; i <= (MAX_NUM_PROCESSES + 1); i++){        //loop through active_proc array.
       if(process == i && active_proc[i] == 1){           //check if you have reached passed process in array.
            process = i+1;
            while(active_proc[i] != 1){                   //make sure next process is active
              if(i = MAX_NUM_PROCESSES + 1){              //if not keep incrementing
                  i = 0;
              }
              i++;
            }
            process = i;
          }
        else
            return -1;
    }
          return process;                                 //return the next active process
}

//TODO still have to figure out what functions are necessary for switching
//     processes, and updating them based on tick frequency.

//     Needs to be done by getting status of the current process being used and
//     looping them using using queue methodology.

//     Probably going to need to figure out how to add new processes into the
//     queue.

void switch_proc(){
    pcb_t* curr;
    curr = get_last_pcb();
    uint8_t curr_pid = curr->pid;

    n_pid = next_process(curr_pid);
    pcb_t* n_pcb = (pcb_t*)(EIGHTMB - ((EIGHTKB)*(n_pid)));


}

void init_terminal_buf(){
    int i;
    for(i = 0; i < PAGE_TABLE_SIZE; i++){
      terminal_page_table[i] = EMPTY_ENTRY;
    }
    page_directory[16] = (uint32_t)terminal_page_table | ENABLE_ENTRY;
    terminal_page_table[0] = (uint32_t)TERM_VID_BUFF | ENABLE_ENTRY;
    terminal_page_table[1] = (uint32_t)(TERM_VID_BUFF + _4KB) | ENABLE_ENTRY;
    terminal_page_table[2] = (uint32_t)(TERM_VID_BUFF + _4KB) | ENABLE_ENTRY;
}
// terminal id is from 0-2
void switch_terminal(uint32_t terminal_id){
    // save real video buffer
    memcpy((TERM_VID_BUFF) + (currenterminal)*_4KB, VIDEO_ADDR, _4KB );
    // change the current terminal to new terminal - global variable
    currenterminal = terminal_id;

    // copy other terminal buffer to new terminal,
    memcpy(VIDEO_ADDR, (TERM_VID_BUFF) + (terminal_id)*_4KB, _4KB );
    // remember about vidmap!!!
    // TODO: Switch video paging
    // VIDEO PAGING: every time you give it a cpu time, set video paging to its corresponding termnials' video physical
    int i;
    for(i=0;i<3;i++){ //loop to update each terminal's physical address for video buffer
      if(i==terminal_id){//if it's terminal that we're switching to
        terminals[i].video_physical = VIDEO_ADDR;
      }
      else{ //set those to "fake" video buffer
        terminals[i].video_physical =  (TERM_VID_BUFF) + (i)*_4KB;
      }
    }
    // TODO: Switch Input keyboard buffer

    // TODO: Update visible video coordinates

}
