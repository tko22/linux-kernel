#include "fs.h"
#include "lib.h"

/* for easy reference
typedef struct dentry_t {
    char file_name[MAX_NAME_LENGTH];
    uint32_t file_type;
    uint32_t inode_num;
    uint8_t reserved[NUM_DENTRY_RESERVED_BYTES]; // reserved 24 bytes
} dentry_t;
*/
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)){
  //find the index by name and then call read_dentry_by_index
  //for loop based on boot first block's # of directory entries
  int i;
  for (i = 0; i < boot_block.num_dir_entries; i++){
    if(strncmp(fname,boot_block.dentries[i].file_name,MAX_NAME_LENGTH) == 0){ // 0 mean no mismatch, i+1 because there is number and reserved for the first entry
      read_dentry_by_index(i,dentry);
      return 0;//success
    }
  }
  return-1;//failure
}

int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
  if(index>boot_block.num_dir_entries-1){// if index isn't less than the number of entires
    return -1; //failure, index out of range
  }
  dentry->file_name = boot_block.dentries[index].file_name;
  dentry->file_type = boot_block.dentries[index].file_type;
  dentry->inode_num = boot_block.dentries[index].inode_num;
  return 0; //success
}

int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
  /*In the case of a file, data should be read to the end of the file or the end of the buffer provided, whichever occurs
sooner. */
  if(inode > boot_block.num_inodes-1){ //
    return -1; //failure, inode index out of range
  }

  //return the number of bytes read
}
