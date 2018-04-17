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
// Boot Block
boot_block_t* boot_block =0x0;
boot_block_t* boot_block_end =0x0;
/*
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


}*/

/* read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
input: filename, dentry to be filled
return 0 for success and -1 for failure
read dentry by the given name
*/
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
  //find the index by name and then call read_dentry_by_index
  //for loop based on boot first block's # of directory entries
  uint32_t fname_length = strlen((char *)fname); //length of the passed filename
  if(fname_length> MAX_NAME_LENGTH ){ //check if the name is 33 long then the last one is null termination
    return -1; //fail, text is too long
  }
  if(fname == NULL || dentry ==NULL || fname_length>MAX_NAME_LENGTH){return -1;}
  int i;
  for (i = 0; i < boot_block->num_dir_entries; i++){
    if(strncmp((char *)fname,boot_block->dentries[i].file_name,fname_length) == 0){ // 0 mean no mismatch
      if(fname_length != 32){
        if (strlen((char*)fname) != strlen(boot_block->dentries[i].file_name)){
          return -1;
        }
      }
      read_dentry_by_index(i,dentry);
      return 0;//success
    }
  }
  return-1;//failure
}
/* int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry)
input: index, dentry to be filled
return 0 for success and -1 for failure
read dentry by the given index
*/
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){

  if (dentry == NULL){ // check for null pointer
    return -1;
  }
  if(index > (boot_block->num_dir_entries)-1){// if index isn't less than the number of entires
    return -1; //failure, index out of range
  }
  //fill the passed dentry with stuff
  //strcpy(dest,source)
  dentry_t_fs* thisdentry;
  thisdentry= &(boot_block->dentries[index]);

  strncpy(dentry->file_name,thisdentry->file_name,MAX_NAME_LENGTH);
  dentry->file_name[MAX_NAME_LENGTH] = '\0';
  dentry->file_type = thisdentry->file_type;
  dentry->inode_num = thisdentry->inode_num;

  return 0; //success
}
/* int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
input: inode index, offset, buffer, length of buffer
output:number of bytes read
read the data to the buffer by specifying inode and byte offset
*/
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
  //In the case of a file, data should be read to the end of the file or the end of the buffer provided, whichever occurs
  //sooner.
  int i;
  uint32_t byteread=0;
  if(inode > boot_block->num_inodes - 1 || buf ==NULL){
    return -1; //failure, inode index out of range
  }
  inode_t* thisinode = ((void*)boot_block + (inode + 1) * BLOCK_SIZE); // pointer to the given inode index
  uint32_t filelength = thisinode->length; // 4 kb file length block
  
  uint32_t read_pointer = offset;

  datablock_t* firstdatablock = ((void*)boot_block + (INODE_NUM+1) * BLOCK_SIZE); //pointer to first datablock

  for(i = 0; i<length; i++){ //for loop through each byte
    if(read_pointer >= filelength) break; //check if we're done reading
    uint32_t reading_block = thisinode->data_block_num[read_pointer/BLOCK_SIZE];//the block that we're reading
    datablock_t* thisdatablock = &firstdatablock[reading_block];
    buf[i] = thisdatablock->data[read_pointer%BLOCK_SIZE]; //copy to buffer
    read_pointer++;
    byteread++;
  }
  return byteread;
  //return the number of bytes read
}

int32_t file_open ( fd_t * fd, const uint8_t* filename){
  // do nothing
  return 0;
}
int32_t file_close (fd_t* fd ){
  // undo what you did in open()
  return 0;
}

int32_t file_read (fd_t* fd, uint8_t* buf, int32_t nbytes){
  // reads count bytes of data from file into buf
  // uses read_data
  if((int32_t)fd < 0){
    return -1;
  }
  int32_t ret = read_data(fd->inode, fd->file_pos, buf, nbytes );
  fd->file_pos = fd->file_pos + ret; // update file_pos
  return ret;
}

int32_t file_write (fd_t* fd, const uint8_t* buf, int32_t nbytes){
  // do nothing
  if((int32_t)fd < 0){
    return -1;
  }
  return -1;
}

// Directory Function
int32_t dir_open (struct fd_t* fd, const uint8_t* filename){
  // opens directory file
  // uses read_dentry_by_index
  return 0;
}
int32_t dir_close (fd_t* fd){
  // "directory close() probably does nothing" - taken directly from discussion slides
  return 0;
}

int32_t dir_read (fd_t* fd, uint8_t* buf, int32_t nbytes){
  // read files filename by filename including
  if((int32_t)fd < 0){
    return -1;
  }
  dentry_t dt;
  int32_t ret = read_dentry_by_index(fd->file_pos, &dt);
  if (ret == -1){
    return 0;
  }
  char * name = dt.file_name;
  int i;
  for (i =0; i < MAX_NAME_LENGTH; i++){
		buf[i] = NULL;
	}
  for (i = 0; i < nbytes; i++){
    if (name[i] != NULL){
      buf[i] = name[i];
    }
    else{
      break;
    }
  }
  fd->file_pos += 1;
  return i;
}

int32_t dir_write (fd_t* fd, const uint8_t* buf, int32_t nbytes){
  // do nothing
  if((int32_t)fd < 0){
    return -1;
  }
  return -1;
}
