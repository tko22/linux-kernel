#include "paging.h"

void init_pages(){
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
