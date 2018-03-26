#include "sys.h"
#include "types.h"
#include "fs.h"

void halt(){
    asm volatile (".1: hlt; jmp .1;");
}
/*int32_t open (const uint8_t* filename){

}
int32_t read (int32_t fd, void* buf, int32_t nbytes){

}
int32_t write (int32_t fd, const void* buf, int32_t nbytes){

}
int32_t close (int32_t fd){

}
*/
