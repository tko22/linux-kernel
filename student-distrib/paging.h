#ifndef _PAGING_H
#define _PAGING_H
#include "types.h"

#define PAGE_DIR_SIZE   1024
#define PAGE_TABLE_SIZE 1024
#define BLANK_PAGE 0x2
extern uint32_t page_entry_desc_t;


// Page 90 of intel descriptor page
typedef union page_entry_desc_t{
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
        uint32_t global_page        : 1;
        uint32_t available          : 3;
        uint32_t page_address       : 20;
    } __attribute__ ((packed));
} page_entry_desc_t;



extern page_entry_desc_t page_directory[PAGE_DIR_SIZE] __attribute__((aligned(4096))); // page directory for 4GB
extern page_entry_desc_t video_page_table[PAGE_TABLE_SIZE] __attribute__((aligned(4096))); // page for the video memory in the 0-4MB

extern void init_pages();
extern void set_cr3();
extern void fill_pages(); // fill page directories and page tables

#endif
