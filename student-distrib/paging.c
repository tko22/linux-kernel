#include "paging.h"

void init_pages(){
    
}






void fill_pages(){
    int i;  
    for ( i = 0; i < PAGE_DIR_SIZE; i++){
        page_directory[i].present  
    }
}
  asm volatile ("                                     \n\
          movl page_directory, %%cr3                  \n\
          movl %%cr0, %%eax                           \n\
          orl %%eax, $0x80000001                      \n\
          movl %%eax, cr0                             \n\
          movl %%cr4, %%eax                           \n\
          orl %%eax, $0x00000010                      \n\
          movl %%eax, %%cr4                           \n\
          "
          :"a"(address), "e"(enable)
          :
          :"memory"
  );
}
