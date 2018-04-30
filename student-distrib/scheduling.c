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

/* void initalize_PIT();
 * Inputs: none
 * Return Value: none
 * Function: Initalize the PIT. set its frequency
 */
void initalize_PIT(){
    outb(0x34, 0x43);
    outb(ThirtyFIVE_HZ,0x40);
    enable_irq(0);
                        //IRQ 0 is for PIT
}

/* void handle_pit_interrupt();
 * Inputs: none
 * Return Value: none
 * Function: gets called everytime PIT sends interrupt. So round robin will ratate every interrupt.
 */
void handle_pit_interrupt(){
  	send_eoi(0);

    switch_proc();                               //call function that does restructuring of the stack
}

/* uint32_t next_process(uint32_t process)
 * Inputs: uint32_t process
 * Return Value: ret
 * Function: Gives the next last active process of the terminals. Cyclying through max of three actives.
 */
uint32_t next_process(uint32_t process){
  uint32_t ret = process + 1;
  ret = (ret - 1) % MAX_NUM_PROCESSES + 1;
  while(active_proc[ret] != 1){        //loop through active_proc array.
     ret++;
     ret = (ret - 1) % MAX_NUM_PROCESSES + 1;
  }
  return ret;
}

/* void switch_proc()
 * Inputs: none
 * Return Value: none
 * Function: switch to the next active process and redo the stack for that process.
   This means loading paging, restoring esp/ebp, etc...
 */
void switch_proc(){
    cli();
    pcb_t* curr;
    curr = get_last_pcb();                                        //get current PCB and PID
    uint32_t curr_pid = curr->pid;

    asm volatile(
                 "movl %%ebp, %0		#Save EBP	\n"
                 "movl %%esp, %1     #Save ESP 	\n"
                 : "=r" (curr->ebp), "=r" (curr->esp)
                 :
                 : "memory"
                 );

    uint32_t n_pid = next_process(curr_pid);                       //get next process ID
    load_program(n_pid);                                           //switch process paging
    pcb_t* n_pcb = (pcb_t*)(EIGHTMB - EIGHTKB*n_pid);       //get the next process block

    //set TSS
    tss.ss0 = KERNEL_DS;                                          // set ss0 to kernel's data segment
    tss.esp0 = FOUR_MB * 2 - KB8 * ((n_pid)-1) -4;                         // set esp0 to the stack (8 MB - PID*8KB)

    asm volatile(                                         //restore the registers for execute
                "movl %0, %%ebp		#Restore EBP	  \n"
                "movl %1, %%esp    #Restore ESP 	\n"
                :
                : "r" (n_pcb->ebp), "r" (n_pcb->esp)
                : "memory"
                );
    sti();
}

/* void init_terminal_buf()
 * Inputs: none
 * Return Value: none
 * Function: initalize the three terminals by setting up three buffers for them.
 */
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

}

/* void switch_terminal(uint32_t terminal_id)
 * Inputs: uint32_t terminal_id
 * Return Value: none
 * Function: When the CRTL+ALT+FN changes terminal, this is the function used to switch terminals. Changes the buffers.
 */
void switch_terminal(uint32_t terminal_id){
    if (currentterminal == terminal_id){
        return;
    }
    // save real video buffer
    memcpy((void*)((TERM_VID_BUFF) + (currentterminal)*_4KB), (void*)VIDEO_ADDR, _4KB );// destination, source
    // change the current terminal to new terminal - global variable
    currentterminal = terminal_id;

    // copy other terminal buffer to new terminal,
    memcpy((void*)VIDEO_ADDR, (void*)((TERM_VID_BUFF) + (terminal_id)*_4KB), _4KB );

    if (terminals[terminal_id].parent_pcb == NULL){
        execute((uint8_t*)"shell");
    }
}
