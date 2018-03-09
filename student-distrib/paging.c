#include "paging.h"

void init_pages(){
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
