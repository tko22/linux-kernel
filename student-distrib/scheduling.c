#include "scheduling.h"
#include "sys.h"
#include "i8259.h"
#include "x86_desc.h"
#include "paging.h"
#include "lib.h"


void initalize_PIT(){
    enable_irq(0); //IRQ 0 is for PIT
    //TODO set the frequency for ticks to use in Round-Robin Method
}
void handle_pit_interrupt(){
  	send_eoi(0);
  // handle PIT here !!, call something
}
uint8_t next_process(curr){
    int i;

    for(i = 1; i <= (MAX_NUM_PROCESSES + 1); i++){
          if(curr == active_proc[i] && i == MAX_NUM_PROCESSES + 1){
            curr = active_proc[1];
          }
          else if(curr == active_proc[i] && i != MAX_NUM_PROCESSES + 1){
            curr = active_proc[i+1];
          }
          else
            return -1;
    }
          return curr;
}


//TODO still have to figure out what functions are necessary for switching
//     processes, and updating them based on tick frequency.

//     Needs to be done by getting status of the current process being used and
//     looping them using using queue methodology.

//     Probably going to need to figure out how to add new processes into the
//     queue.
void switch_proc(){
    cli();
    pcb_t * caller_pcb;
    caller_pcb=get_last_pcb();


}
