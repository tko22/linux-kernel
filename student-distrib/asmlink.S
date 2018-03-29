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
  call handle_rtc_interrupt
  popa      # pop all register
  iret

.globl linkSystemCall
linkSystemCall:
  pusha
  cmpl $1,%eax
  jl invalid_number  # check if code is between 1-10
  cmpl $10,%eax
  jg invalid_number
  call *syscall_jumptable(,%eax,4)  # if systemcall number is valid call using jumptable
  jmp syscall_done
invalid_number:
  movl $-1,%eax      # return -1 for invalid syscall number
syscall_done:
  popa
  iret


syscall_jumptable:
  .long halt,execute,read,write,open,close
.end
