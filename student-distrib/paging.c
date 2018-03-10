#include "paging.h"

void init_pages(){
    // stores start of page directory table in cr3
    // sets the page enable bit and protected mode enable bit to high in cr0
    // enables 4 MB pages (PSE) by modifying cr4
    asm volatile ("                                   \n\
          movl %%cr0, %%eax                           \n\
          orl %%eax, $0x80000001                      \n\
          movl %%eax, %%cr0                           \n\
          movl %%cr4, %%eax                           \n\
          orl %%eax, $0x00000010                      \n\
          movl %%eax, %%cr4                           \n\
          "
  );
}

void set_cr3(){
  asm volatile("movl page_directory, %%cr3");
}






void fill_pages(){
    int i;
    // set up 0-4MB Page
    /*page_directory[0].present = 1;
    page_directory[0].read_or_write = 1;


    // Set up 4-8MB Page
    page_directory[1].present = 1;
    page_directory[1].read_or_write = 1;



    for (i = 2; i < PAGE_DIR_SIZE; i++){
        // set everything else to have present 0
        page_directory[i].present = 0;

    }*/
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
    page_table[VIDEO_ADDR / PAGE_TABLE_SIZE] = VIDEO_ADDR | 3;
    page_directory[0] = page_table | 3;
    page_directory[1] = KERNEL | ENABLE_4MBYTE_PAGE | 3;
}
