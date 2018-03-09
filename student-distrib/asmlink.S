#include "asmlink.h"

.globl linkKeyboard
linkKeyboard:
  pusha     # push all registers
  call handle_keyboard_interrupt
  popa      # pop all register
  iret
  
.globl linkRTC
linkRTC:
  pusha     # push all registers
  call test_interrupts
  popa      # pop all register
  iret
