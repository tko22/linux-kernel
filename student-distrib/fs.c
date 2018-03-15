#include "fs.h"
#include "lib.h"
#include "keyboard.h"
#include "file_desc.h"
/* from the discussion slide
File open() initialize any temporary structures, return 0
- Uses read_dentry_by_name: name means filename
File close() undo what you did in the open function, return 0
File write() should do nothing, return -1
File read() reads count bytes of data from file into buf
- Uses read_data
*/

void init_fs(){
  // stdin
  file_array[0].file_op_table_pointer = 0x00; // idk what this should be
  file_array[0].file_pos = 0; // not sure
  file_array[0].flags = 1; // in use
  file_array[0].inode = 1; // should be 0 for directories and RTC, so 1?

  // stdout
  file_array[1].file_op_table_pointer = 0x00;
  file_array[1].file_pos = 0;
  file_array[1].flags = 1;  // in use
  file_array[1].inode = 1; // should be 0 for directories and RTC, so 1?


}
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
  if(fname == NULL || dentry ==NULL){return -1;}
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
  if(dentry ==NULL){return -1;}// check for null pointer
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
  int i;
  if(inode > boot_block.num_inodes-1||buf ==NULL){ //
    return -1; //failure, inode index out of range
  }
  uint32_t inodeblock = (boot_block + (inode + 1) * BLOCK_SIZE);// get length fron the given inode block
  uint32_t filelength = *inodeblock; // 4 kb file length block
  uint32_t firstdatablock = *(boot_block + INODE_NUM * BLOCK_SIZE);
  for(i=offset;(i<filelength && i<length);i++){
    buf[i] = *(firstdatablock+(*(inodeblock+(i+1)*4))*BLOCK_SIZE); // firstdatablock + datablocknumber * size of datablock
  }
  return i;
  //return the number of bytes read
}
int32_t file_open (const uint8_t* filename){

}
int32_t file_read (void* buf, int32_t nbytes){

}
int32_t file_write (const void* buf, int32_t nbytes){

}
int32_t file_close (void){

}

int32_t dir_open (const uint8_t* filename){

}
int32_t dir_read (void* buf, int32_t nbytes){

}
int32_t dir_write (const void* buf, int32_t nbytes){

}
int32_t dir_close (void){

}
