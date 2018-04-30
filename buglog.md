# Bug Log
## Checkpoint 1
- Problem: interrupts wouldn’t get handled Fix: we forgot to set the present bit in the entries of IDT. In addition we forgot to clear interrupts in the interrupt handlers
- Problem: The PIC was not properly enabling and disabling interrupts. Fix: We were not properly bit shift the parameters to set certain bits to zero and one. 
- Problem: Paging wasn’t setting up correctly. It didn’t seem like it was even running. Fix: We had to set cr3
- Problem: interrupts weren’t firing Fix: we forgot to set sti() in kernel.c
## Checkpoint 2
- Problem: RTC would not receive interrupts at the proper rate with the frequency passed in write. Fix: We were setting to the rate to be the frequency, but after going over Intel documentation, we saw that each frequency value corresponds to a hex rate value. 
- Problem: One of our tests functions always page faulted for read_data(). Fix: we spent 3 hrs trying to figure it out but it was because we used `%x` inside a print statement when the variable was a character array
## Checkpoint 3
- Problem: ls wasn’t printing all the files. Fix: Directory read was not changing file_position so made it do that. Except we purposely skipped `.` because in unix `ls` skips `.` but we got points off of that
- Problem: Halt would not properly decrement total processes being run. Fix: Testing between using an array of active process or a counter that keeps track of total processes. Decided to use a counter. 
- Problem: Shell wasn’t executing Fix: we didn’t call execute shell in kernel.c…. Of course it did not execute haha
## Checkpoint 4
- Problem: Pingpong would not run after first interrupt. Fix: Rtc_read was not getting interrupts because of problems with trap gate, so changed it to correct values. 
- Problem: open would not check if files have already been opened. Fix: Set conditional to check, so error message was not received. 
- Problem: getArgs wasn’t working Fix: we were only storing one buffer for arguments when it’s dependent on each process
## Checkpoint 5
- Problem: Program terminated abnormally when running `hello`, `shell`. Fix: We were saving esp and ebp to new pcb in execute instead of saving it to the parent pcb. We were also doing that in halt.
- Problem: Page faults when running a user program after multiple processes were started. Fix: We were saving and getting esp0 incorrectly, we had it in the same location as pcb of the previous process, We also had incorrect indexing for pids and mapping to the correct pcb, indexed from 0 instead of 1
- Problem: Page faulting when switching terminals while running pingpong or counter Fix: Scrollx and scrolly go out of bounds, had to fix terminal_read
- Problem: Switching terminals mapped to incorrect buffer
- Problem: Pingpong and rtc would work. They would stop after the first frame. Fix: Go back to using interrupt gate, so need to use sti() again in rtc_read. 
- Problem:Program crashes when you run more than 6 program because we  were using a number to give processes their id instead of storing it in an array. Fix. count the number of process from an array instead 
