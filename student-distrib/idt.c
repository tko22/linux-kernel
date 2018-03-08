#include "x86_desc.h"
#include "idt.h"

void init_idt(){
    int i;
    // - From 0 to 31   : exceptions and non-maskable interrupts
    // - From 32 to 47  : maskable interrupts
    // - From 48 to 255 : software interrupts

    // 32 Exceptions to handle
    
    for(i = 0; i < 32; i++) {
        // For easy look up
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

        // i dont know whether these are all correct
        // I couldn't find where the interrupt entry descriptors
        // were described in the intel manual
        // so I referenced this http://www.independent-software.com/writing-your-own-toy-operating-system-setting-up-the-interrupt-descriptor-table-idt/
        // oh well, hopefully it's right

        idt[i].seg_selector = KERNEL_CS; // not sure, either KERNEL_CS or KERNEL_DS
        idt[i].reserved4 = 0;  // its 0 because it says its zero - unused

        // The next 2 bytes are type/attribute
        // reserved 1-3 are gate types - I'm pretty sure that 
        // exceptions use the 32bit trap gate, which is 0xf
        idt[i].reserved3 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1; 

        // must be set to 0 for interrupt gates but its not an interrupt gate!
        // because it is an exception!
        idt[i].size = 1;  
        // apparently, bits 5-6 are for dpl, but only bit 6 is named dpl
        // so I guess I'll put 0 done for reserved 0
        idt[i].reserved0 = 0;
        idt[i].dpl = 0;     // exception handlers must have dpl = 0 - for sure right
        
        idt[i].present = 1; // empty descriptor slots have 0, else 1 - for sure right - normally 1

    }
}