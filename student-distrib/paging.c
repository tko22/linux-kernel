#include "paging.h"

void init_pages(){
    // stores start of page directory table in cr3
    // sets the page enable bit and protected mode enable bit to high in cr0
    // enables 4 MB pages (PSE) by modifying cr4
    asm volatile ("                                   \n\
          movl page_directory, %%cr3                  \n\
          movl %%cr0, %%eax                           \n\
          orl %%eax, $0x80000001                      \n\
          movl %%eax, %%cr0                           \n\
          movl %%cr4, %%eax                           \n\
          orl %%eax, $0x00000010                      \n\
          movl %%eax, %%cr4                           \n\
          "
          :"a"(address), "e"(enable)
          :
          :"memory"
  );
}






void fill_pages(){
    int i;
    // set up 0-4MB Page
    page_directory[0].present = 1;
    page_directory[0].read_or_write = 1;


    // Set up 4-8MB Page
    page_directory[1].present = 1;
    page_directory[1].read_or_write = 1;



    for (i = 2; i < PAGE_DIR_SIZE; i++){
        // set everything else to have present 0
        page_directory[i].present = 0;

    }
}
