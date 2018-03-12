#include "sys.h"

void halt(){
    asm volatile (".1: hlt; jmp .1;");
}
