#include "scheduling.h"
#include "sys.h"
#include "i8259.h"
#include "x86_desc.h"
#include "paging.h"
#include "lib.h"
#include "file_desc.h"

#define TERM_VID_BUFF 0x4000000
#define EIGHTKB 0x2000      //2^13
#define EIGHTMB 0x00800000

volatile int shells = 1;

void initalize_PIT(){
    outb(0x34, 0x43);
    outb(ThirtyFIVE_HZ,0x40);
    enable_irq(0);
                        //IRQ 0 is for PIT
}

void handle_pit_interrupt(){
  	send_eoi(0);
    // handle PIT here !!, call something
    // pcb_t* curr;
    // curr = get_last_pcb();                      //get current process
    // process = curr->pid;
    // next_process(process);                      //get next process
    
    switch_proc();                               //call function that does restructuring of the stack
}

uint32_t next_process(uint32_t process){
  uint32_t ret = process + 1;
  ret = (ret - 1) % MAX_NUM_PROCESSES + 1;
  while(active_proc[ret] != 1){        //loop through active_proc array.
     ret++;
     ret = (ret - 1) % MAX_NUM_PROCESSES + 1;
  }
  return ret;
}

void switch_proc(){
    cli();
    pcb_t* curr;
    curr = get_last_pcb();
    uint32_t curr_pid = curr->pid;

    asm volatile(
                 "movl %%ebp, %0		#Save EBP	\n"
                 "movl %%esp, %1     #Save ESP 	\n"
                 : "=r" (curr->ebp), "=r" (curr->esp)
                 :
                 : "memory"
                 );
    if (shells < 3){
         terminals[shells].bufferPos = 0;
         terminals[shells].currentcolumn = 0;
         terminals[shells].currentrow = 0;
         terminals[shells].terminalrow = 0;
         terminals[shells].terminalcol = 0;
         int j;
         for(j = 0; j < 128; j++){
               terminals[shells].keyboardbuffer[j] = '\0';
         }
         currentterminal = shells;
         terminals[shells].parent_pcb = NULL;
         shells += 1;
         execute((uint8_t*)"shell");
    }
    uint32_t n_pid = next_process(curr_pid);                       //get next process ID
    load_program(n_pid);                                           //switch process paging
    pcb_t* n_pcb = (pcb_t*)(EIGHTMB - EIGHTKB*n_pid);       //get the next process block
    // set video address
    // page_table[VIDEO_ADDR / _4KB] = terminals[n_pcb->terminal_id].video_physical | ENABLE_ENTRY;
    // asm volatile("movl %%cr3, %%eax;" "movl %%eax, %%cr3;" ::: "eax"); //flush tlb
    //set TSS
    tss.ss0 = KERNEL_DS;                                          // set ss0 to kernel's data segment
    tss.esp0 = FOUR_MB * 2 - KB8 * n_pid;                         // set esp0 to the stack (8 MB - PID*8KB)

    //set EBP and ESP to the next process (the process we're switching too)
    asm volatile(                                         //restore the registers for execute
                "movl %0, %%ebp		#Restore EBP	  \n"
                "movl %1, %%esp    #Restore ESP 	\n"
                :
                : "r" (n_pcb->ebp), "r" (n_pcb->esp)
                : "memory"
                );
    sti();
}

void init_terminal_buf(){
    int i;
    for(i = 0; i < PAGE_TABLE_SIZE; i++){
      terminal_page_table[i] = EMPTY_ENTRY;
    }
    page_directory[16] = (uint32_t)terminal_page_table | ENABLE_ENTRY;
    terminal_page_table[0] = (uint32_t)(TERM_VID_BUFF) | ENABLE_ENTRY;
    terminal_page_table[1] = (uint32_t)(TERM_VID_BUFF + _4KB) | ENABLE_ENTRY;
    terminal_page_table[2] = (uint32_t)(TERM_VID_BUFF + _4KB + _4KB) | ENABLE_ENTRY;
    asm volatile("movl %%cr3, %%eax;" "movl %%eax, %%cr3;" ::: "eax"); //flush tlb
    // terminals[0].video_physical = TERM_VID_BUFF;
    // terminals[1].video_physical =  (TERM_VID_BUFF) + _4KB;
    // terminals[2].video_physical =  (TERM_VID_BUFF) + 2*_4KB;
    // asm volatile("movl %%cr3, %%eax;" "movl %%eax, %%cr3;" ::: "eax"); //flush tlb
}
// terminal id is from 0-2
void switch_terminal(uint32_t terminal_id){
    printf("Switching Terminals to %d \n", terminal_id );
    // save real video buffer
    memcpy((void*)((TERM_VID_BUFF) + (currentterminal)*_4KB), (void*)VIDEO_ADDR, _4KB );// destination, source
    // change the current terminal to new terminal - global variable
    currentterminal = terminal_id;

    // copy other terminal buffer to new terminal,
    memcpy((void*)VIDEO_ADDR, (void*)((TERM_VID_BUFF) + (terminal_id)*_4KB), _4KB );
    // remember about vidmap!!!
    // TODO: Switch video paging
    // page_table[VIDEO_ADDR / _4KB] = terminals[terminal_id].video_physical | ENABLE_ENTRY;
    // VIDEO PAGING: every time you give it a cpu time, set video paging to its corresponding termnials' video physical
    // int i;
    // for(i=0;i<3;i++){ //loop to update each terminal's physical address for video buffer
    //   if(i==terminal_id){//if it's terminal that we're switching to
    //     //terminals[i].video_physical = VIDEO_ADDR;
    //     terminal_page_table[terminal_id] = VIDEO_ADDR;
    //   }
    //   else{ //set those to "fake" video buffer
    //     terminal_page_table[i]  =  (TERM_VID_BUFF) + (i)*_4KB;
    //   }
    // }
    // asm volatile("movl %%cr3, %%eax;" "movl %%eax, %%cr3;" ::: "eax"); //flush tlb
    // TODO: Switch Input keyboard buffer

    // TODO: Update visible video coordinates

}
