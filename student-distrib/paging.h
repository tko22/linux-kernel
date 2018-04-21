#ifndef _PAGING_H
#define _PAGING_H
#include "types.h"

#define PAGE_DIR_SIZE   1024
#define PAGE_TABLE_SIZE 1024
#define _4KB 4096
#define VIDEO_ADDR 0xB8000
#define KERNEL 0x400000
#define ENABLE_4MBYTE_PAGE 0x80
#define VIDEO_MEM_PAGE_ADDR  184
#define EMPTY_ENTRY 0x2
#define ENABLE_ENTRY 3
#define _4MB 0x400000
#define ENABLE_ENTRY_USER 7
//extern uint32_t page_entry_desc_t;


// Page 90 of intel descriptor page
/*typedef union page_entry_desc_t{
    uint32_t val;
    struct {
        uint32_t present            : 1;    // bit 0
        uint32_t read_or_write      : 1;    // bit 1
        uint32_t user__or_super     : 1;
        uint32_t write_through      : 1;
        uint32_t cache_disable      : 1;
        uint32_t accessed           : 1;
        uint32_t dirty              : 1;
        uint32_t page_table_index   : 1;
        uint32_t global_page        : 1;    // bit 7
        uint32_t available          : 3;    // bits 8-11
        uint32_t address            : 20;
    } __attribute__ ((packed));
} page_entry_desc_t;*/



//page_entry_desc_t page_directory[PAGE_DIR_SIZE] __attribute__((aligned(_4KB))); // page directory for 4GB
//page_entry_desc_t page_table[PAGE_TABLE_SIZE] __attribute__((aligned(_4KB))); // page for the video memory in the 0-4MB

uint32_t page_directory[PAGE_DIR_SIZE] __attribute__((aligned(_4KB))); // page directory for 4GB
uint32_t page_table[PAGE_TABLE_SIZE] __attribute__((aligned(_4KB))); // page for the video memory in the 0-4MB
uint32_t terminal_page_table[PAGE_TABLE_SIZE] __attribute__((aligned(_4KB))); // terminal page table

// initialize pages
extern void init_pages();
// set cr3
extern void set_cr3(uint32_t* addr);
// fill pages
extern void fill_pages(); // fill page directories and page tables

extern void load_program(uint32_t process);

extern uint8_t* init_vidmap(uint32_t process);

#endif
