#include "x86_desc.h"
#include "idt.h"
#include "keyboard.h"
#include "rtc.h"
#include "lib.h"
#include "sys.h"
/* init_idt()
 *  Initialize the idt table. Sets the correct value of idt entry descriptors
 *  Look at page 150 in ISA for more information about the idt
 *  entry descriptors
 */
void init_idt(){
    int i;
    // - From 0 to 31   : exceptions and non-maskable interrupts
    // - From 32 to 47  : maskable interrupts
    // - From 48 to 255 : software interrupts

    // 32 Exceptions to handle
    for(i = 0; i < NUM_RESERVED; i++) {
        // For easy look up - LEAST SIGNIFICANT BIT
        // uint16_t offset_15_00;
        // uint16_t seg_selector;
        // uint8_t  reserved4;
        // uint32_t reserved3 : 1;
        // uint32_t reserved2 : 1;
        // uint32_t reserved1 : 1;
        // uint32_t size      : 1;
        // uint32_t reserved0 : 1;
        // uint32_t dpl       : 2;
        // uint32_t present   : 1;
        // uint16_t offset_31_16;
        // THIS IS THE 64th BIT

        // i dont know whether these are all correct
        // I couldn't find where the interrupt entry descriptors
        // were described in the intel manual
        // so I referenced this http://www.independent-software.com/writing-your-own-toy-operating-system-setting-up-the-interrupt-descriptor-table-idt/
        // oh well, hopefully it's right
        // jk, it's in page 156 of the intel handbook
        if (i == 15){
            continue;
        }
        idt[i].seg_selector = KERNEL_CS; // not sure, either KERNEL_CS or KERNEL_DS
        idt[i].reserved4 = 0;  // 0-7 bits is 0

        // The next 2 bytes are type/attribute
        // reserved 1-3 are gate types - I'm pretty sure that
        // exceptions use the 32bit trap gate, which is 0xf
        idt[i].reserved3 = 0;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;

        // must be set to 0 for interrupt gates but its not an interrupt gate!
        // because it is an exception!
        idt[i].size = 1;   // 32 bits
        idt[i].reserved0 = 0;
        // apparently, bits 5-6 are for dpl
        idt[i].dpl = 0;     // exception handlers must have dpl = 0 - for sure right

        idt[i].present = 1; // empty descriptor slots have 0, else 1 - for sure right - normally 1
    }
    SET_IDT_ENTRY(idt[0],handle_divide_error);
    SET_IDT_ENTRY(idt[1],handle_debug_exception);
    SET_IDT_ENTRY(idt[2],handle_nmi_interrupt);
    SET_IDT_ENTRY(idt[3],handle_breakpoint);
    SET_IDT_ENTRY(idt[4],handle_overflow);
    SET_IDT_ENTRY(idt[5],handle_bound_range_exceeded);
    SET_IDT_ENTRY(idt[6],handle_invalid_opcode);
    SET_IDT_ENTRY(idt[7],handle_device_not_avail);
    SET_IDT_ENTRY(idt[8],handle_double_fault);
    SET_IDT_ENTRY(idt[9],handle_coprocessor_seg);
    SET_IDT_ENTRY(idt[10],handle_invalid_tss);
    SET_IDT_ENTRY(idt[11],handle_seg_not_present);
    SET_IDT_ENTRY(idt[12],handle_stack_seg_fault);
    SET_IDT_ENTRY(idt[13],handle_general_protection);
    SET_IDT_ENTRY(idt[14],handle_page_fault);
    SET_IDT_ENTRY(idt[16],handle_FPU_floating_point_error);
    SET_IDT_ENTRY(idt[17],handle_alignment_check_exception);
    SET_IDT_ENTRY(idt[18],handle_machine_check_exception);
    SET_IDT_ENTRY(idt[19],handle_SIMD_floating_point_exception);

    // I'm not sure whether we fill in all
    for (i = NUM_RESERVED; i < NUM_VEC; i++){
        idt[i].seg_selector = KERNEL_CS; // not sure, either KERNEL_CS or KERNEL_DS
        idt[i].reserved4 = 0;  // 0-7 bits is 0

        if ( i != 0x80) { // check if its the vector for system calls
            // The next 2 bytes are type/attribute
            // reserved 1-3 are gate types - using interrupt gate which is 0xe
            idt[i].reserved3 = 0;
            idt[i].reserved2 = 1;
            idt[i].reserved1 = 1; // assuming reserved1 is the least significant bit???
            // apparently, bits 5-6 are for dpl
            // Hardware interrupt handlers must be 0 to prevent user-level applications
            // from calling these routines with the int instruction
            idt[i].dpl = 0;     // exception handlers must have dpl = 0 - for sure right
            idt[i].size = 1;
        }
        else {
            // System Calls
            // trap gate
            idt[i].reserved3 = 1;
            idt[i].reserved2 = 1;
            idt[i].reserved1 = 1;
            // system calls have dpl 3
            idt[i].dpl = 3;
            idt[i].size = 1;
        }

        idt[i].reserved0 = 0;
        // handle System Calls
        // I guess we will set everything to present - idk
        idt[i].present = 1; // empty descriptor slots have 0, else 1 - for sure right - normally 1
    }
    // Now, we handle keyboard, rtc, and pic interrupts
    SET_IDT_ENTRY(idt[0x21],linkKeyboard); // handle keyboard
    SET_IDT_ENTRY(idt[0x28],linkRTC);   // handle rtc
    SET_IDT_ENTRY(idt[0x80],linkSystemCall); //link systemcall
}

// Interrupt/Exception Handlers

// vec 0, handle divide error, input:none, return:none function: printout and handle divide error
void handle_divide_error(){
  cli();
  printf("divide error\n");

  halt(1);
}
// vec 1, handle debug exception, input:none, return:none function: printout and handle debug exception
void handle_debug_exception(){
  cli();
  printf("debug exception\n");

  halt(1);
}
// vec 2, handle nmi interrupt, input:none, return:none function: printout and handle nmi interrupt
void handle_nmi_interrupt(){
  cli();
  printf("nmi interrupt\n");

  halt(1);
}
// vec 3, handle breakpoint, input:none, return:none function: printout and handle breakpoint
void handle_breakpoint(){
  cli();
  printf("handle_breakpoint\n");

  halt(1);
}
// vec 4 handle overflow, input:none, return:none function: printout and handle overflow
void handle_overflow(){
  cli();
  printf("overflow\n");

  halt(1);
}
// vec 5 handle bound range exceeded  input:none, return:none function: printout and handle bound range exceeded
void handle_bound_range_exceeded(){
  cli();
  printf("bound range exceeded\n");

  halt(1);
}
// vec 6 handle invalid opcode,  input:none, return:none function: printout and handle invalid opcode
void handle_invalid_opcode(){
  cli();
  printf("invalid opcode\n");

  halt(1);
}
// vec 7 handle device not available,  input:none, return:none function: printout and handle device not available
void handle_device_not_avail(){
  cli();
  printf("device not available\n");

  halt(1);
}
// vec 8 handle double fault  input:none, return:none function: printout and handle double fault
void handle_double_fault(){
  cli();
  printf("double_fault\n");

  halt(1);
}
// vec 9  handle coprocessor seg input:none, return:none function: printout and handle coprocessor seg
void handle_coprocessor_seg(){
  cli();
  printf("coprocessor seg\n");

  halt(1);
}
// vec 10 handle invalid tests input:none, return:none function: printout and handle invalid tss
void handle_invalid_tss(){
  cli();
  printf("invalid tss\n");

  halt(1);
}
// vec 11 handle segment not present  input:none, return:none function: printout and handle seg not present
void handle_seg_not_present(){
  cli();
  printf("seg not present\n");

  halt(1);
}
// vec 12 handle stack seg fault  input:none, return:none function: printout and handle stack seg fault
void handle_stack_seg_fault(){
  cli();
  printf("seg fault\n");

  halt(1);
}
// vec 13  handle general exception input:none, return:none function: printout and handle general protection
void handle_general_protection(){
  cli();
  printf("general protection\n");

  halt(1);
}
// vec 14  handle page fault input:none, return:none function: printout and handle page fault
void handle_page_fault(){
  cli();
  printf("page fault\n");

  halt(1);
}

// vec 16 handle FPU floating point error, input:none, return none, function:handle floating point error
void handle_FPU_floating_point_error(){
  cli();
  printf("FPU floating point error\n");

  halt(1);
}
//vec 17 handle alignment check exception  input:none, return:none function: printout and handle alignment check exception
void handle_alignment_check_exception(){
  cli();
  printf("alignment check exception\n");

  halt(1);
}
//vec 18 handle machine check exception,  input:none, return:none function: printout and handle machine check exception
void handle_machine_check_exception(){
  cli();
  printf("machine check exception\n");

  halt(1);
}
//vec 19 handle SIMT floating point exception,  input:none, return:none function: printout and handle SIMT floating point exception
void handle_SIMD_floating_point_exception(){
  cli();
  printf("SIMD floating point exception\n");

  halt(1);
}
//vec 32-225 , user defined interrupts
