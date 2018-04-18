#ifndef _IDT_H
#define _IDT_H

#define NUM_RESERVED       32

void init_idt();
void handle_divide_error();
void handle_debug_exception();
void handle_nmi_interrupt();
void handle_breakpoint();
void handle_overflow();
void handle_bound_range_exceeded();
void handle_invalid_opcode();
void handle_device_not_avail();
void handle_double_fault();
void handle_coprocessor_seg();
void handle_invalid_tss();
void handle_seg_not_present();
void handle_stack_seg_fault();
void handle_general_protection();
void handle_page_fault();
void handle_FPU_floating_point_error();
void handle_alignment_check_exception();
void handle_machine_check_exception();
void handle_SIMD_floating_point_exception();

void linkKeyboard();
void linkRTC();
void linkSystemCall();
void linkPIT();
#endif
