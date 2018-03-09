#include "paging.h"

void init_pages(){
<<<<<<< HEAD
    
}






void fill_pages(){
    int i;  
    for ( i = 0; i < PAGE_DIR_SIZE; i++){
        page_directory[i].present  
    }
}
=======
  asm volatile ("                                     \n\
          movl page_directory, %%cr3                  \n\
          movl %%cr0, %%eax                           \n\
          orl %%eax, $0x8                             \n\
          movl %%eax, cr0                             \n\
          "
          :"a"(address), "e"(enable)
          :
          :"memory"
  );
}
>>>>>>> 53069865ecafb09e26a518c2ecbf5c17f52cd788
