#include "paging.h"
#include "types.h"

void init_pages(){
    // stores start of page directory table in cr3
    // sets the page enable bit and protected mode enable bit to high in cr0
    // enables 4 MB pages (PSE) by modifying cr4
    asm volatile ("                                   \n\
          movl %cr0, %eax                           \n\
          or $0x80000001, %eax                      \n\
          movl %eax, %cr0                           \n\
          movl %cr4, %eax                           \n\
          or $0x00000010, %eax                      \n\
          movl %eax, %cr4                           \n\
          "
  );
}

void set_cr3(uint32_t addr){
  asm volatile("movl %%eax, %%cr3"
				:
				:"a"(addr)
  );
}






void fill_pages(){
    int i;
    //set each entry to not present
    for(i = 0; i < PAGE_DIR_SIZE; i++){
      // This sets the following flags to the pages:
      //   Supervisor: Only kernel-mode can access them
      //   Write Enabled: It can be both read from and written to
      //   Not Present: The page table is not present
      page_directory[i] = BLANK_PAGE;
    }
    for(i = 0; i < PAGE_TABLE_SIZE; i++){
      // This sets the following flags to the pages:
      //   Supervisor: Only kernel-mode can access them
      //   Write Enabled: It can be both read from and written to
      //   Not Present: The page table is not present
      page_table[i] = BLANK_PAGE;
    }
    // creates the video memory page and enables it
    page_table[VIDEO_ADDR / _4KB] = VIDEO_ADDR | 3;
    // first page directory points to page table with video memory in it
    page_directory[0] = (uint32_t)page_table | 3;
    // second page directory points to kernel 4 MB page
    page_directory[1] = KERNEL | ENABLE_4MBYTE_PAGE | 3;
}
