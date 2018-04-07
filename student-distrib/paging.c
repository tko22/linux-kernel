#include "paging.h"
#include "types.h"

/*
 * init_pages
 *   DESCRIPTION: enables global paging in cr4 register, enables paging
 *                in cr0 register, and stores address of directory in cr3 register
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */

void init_pages(){
    // stores start of page directory table in cr3
    // sets the page enable bit and protected mode enable bit to high in cr0
    // enables 4 MB pages (PSE) by modifying cr4
	set_cr3(page_directory);
    asm volatile ("movl %%cr4, %%eax                  \n\
          orl $0x00000010, %%eax                      \n\
          movl %%eax, %%cr4                           \n\
          movl %%cr0, %%eax                           \n\
          orl $0x80000000, %%eax                      \n\
          movl %%eax, %%cr0                           \n\
          "
		  :
		  :
		  :"eax"
  );
}

/*
 * set_cr3
 *   DESCRIPTION: stores start of page directory in cr3
 *   INPUTS: addr of page directory
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */

void set_cr3(uint32_t* addr){
	if(addr == NULL){
		return;
	}
  asm volatile("movl %%eax, %%cr3"
				:
				:"a"(addr)
  );
}




/*
 * fill_pages
 *   DESCRIPTION: Fills the page directory and first table with empty entries, then enables video
 *                memory. Fills the first 2 page directories with table with VGA page and 4 MB page
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */

void fill_pages(){
    int i;
    //set each entry to not present
    for(i = 0; i < PAGE_DIR_SIZE; i++){
      // This sets the following flags to the pages:
      //   Supervisor: Only kernel-mode can access them
      //   Write Enabled: It can be both read from and written to
      //   Not Present: The page table is not present
      page_directory[i] = EMPTY_ENTRY;
	    //page_directory[i].read_or_write = 1;
    }
    for(i = 0; i < PAGE_TABLE_SIZE; i++){
      // This sets the following flags to the pages:
      //   Supervisor: Only kernel-mode can access them
      //   Write Enabled: It can be both read from and written to
      //   Not Present: The page table is not present
      page_table[i] = EMPTY_ENTRY;
    }
    // creates the video memory page and enables it
    page_table[VIDEO_ADDR / _4KB] = VIDEO_ADDR | ENABLE_ENTRY;
    // first page directory points to page table with video memory in it
    page_directory[0] = (uint32_t)page_table | ENABLE_ENTRY;
    // second page directory points to kernel 4 MB page
    page_directory[1] = KERNEL | ENABLE_4MBYTE_PAGE | ENABLE_ENTRY;
}

void load_program(uint32_t process){
  uint32_t start = process * _4MB + KERNEL;
  page_directory[32] = start | ENABLE_4MBYTE_PAGE | ENABLE_ENTRY_USER;
  asm volatile("movl %%cr3, %%eax;" "movl %%eax, %%cr3;" ::: "eax");
}
