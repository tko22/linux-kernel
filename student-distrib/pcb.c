#include "pcb.h"

pcb_t pcb_init(){
  //initialize a new pcb and returns it with everything set to NULL or 0
  int i;
  pcb_t new_pcb;
  for(i = 0; i < FD_ARRAY_SIZE; i++){
    new_pcb.fd_arr[i].file_op_table_pointer = NULL;
    new_pcb.fd_arr[i].inode = NULL;
    new_pcb.fd_arr[i].file_pos = NULL;
    new_pcb.fd_arr[i].flags = 0;
    // new_pcb.fd_arr[i] = NULL;
  }
  for(i = 0; i < 128; i++){
    new_pcb.argsbuffer[i] = '\0';
  }
  new_pcb.parent = NULL;
  new_pcb.pid = 0;
  new_pcb.esp = 0;
  new_pcb.ebp = 0;
  new_pcb.cr3 = 0;
  new_pcb.esp0 = 0;
  new_pcb.ss0 =0;
  new_pcb.status = 0;
  return new_pcb;
}
