#include "pcb.h"

pcb_t pcb_init(){
  int i;
  pcb_t new_pcb;
  for(i = 0; i < FD_ARRAY_SIZE; i++){
    // new_pcb.fd_arr[i].file_op_table_pointer = NULL;
    // new_pcb.fd_arr[i].inode = 0;
    // new_pcb.fd_arr[i].file_pos = 0;
    // new_pcb.fd_arr[i].flags = 0;
    new_pcb.fd_arr[i] = NULL;
  }
  new_pcb.parent = NULL;
  new_pcb.pid = 0;
  return new_pcb;
}
