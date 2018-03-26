#ifndef _FS_H
#define _FS_H

#include "types.h"
#include "file_desc.h"

#define MAX_NAME_LENGTH                 32
#define NUM_DENTRY_RESERVED_BYTES       24
#define NUM_BOOT_BLOCK_RESERVED_BYTES   52
#define NUM_BOOT_DENTRIES               63
#define NUM_DATA_BLOCK                  1023
#define BLOCK_SIZE                      4096 // 4 KB
#define INODE_NUM                       64
#define FD_ARRAY_SIZE                   8


typedef struct dentry_t {
  char file_name[MAX_NAME_LENGTH];
  uint32_t file_type;
  uint32_t inode_num;
  uint8_t reserved[NUM_DENTRY_RESERVED_BYTES]; // reserved 24 bytes
} dentry_t;

typedef struct boot_block_t {
  uint32_t num_dir_entries;
  uint32_t num_inodes;
  uint32_t num_data_blocks;
  uint8_t reserved[NUM_BOOT_BLOCK_RESERVED_BYTES];
  dentry_t dentries[NUM_BOOT_DENTRIES];
} boot_block_t;

typedef struct inode_t {
  int32_t length;
  int32_t data_block_num[NUM_DATA_BLOCK];
} inode_t;

typedef struct datablock_t{
  uint8_t data[BLOCK_SIZE];
} datablock_t;

// Boot Block
boot_block_t* boot_block =0x0;
boot_block_t* boot_block_end =0x0;
extern void init_fs();
extern int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
extern int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
extern int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

extern int32_t file_open (const uint8_t* filename);
extern int32_t file_read (void* buf, int32_t nbytes);
extern int32_t file_write (const void* buf, int32_t nbytes);
extern int32_t file_close (void);

extern int32_t dir_open (const uint8_t* filename);
extern int32_t dir_read (void* buf, int32_t nbytes);
extern int32_t dir_write (const void* buf, int32_t nbytes);
extern int32_t dir_close (void);
#endif
