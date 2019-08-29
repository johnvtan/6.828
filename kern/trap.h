/* See COPYRIGHT for copyright information. */

#ifndef JOS_KERN_TRAP_H
#define JOS_KERN_TRAP_H
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

#include <inc/trap.h>
#include <inc/mmu.h>

#define GATEDESC_TRAP (1)
#define GATEDESC_INT (0)

/* The kernel's interrupt descriptor table */
extern struct Gatedesc idt[];
extern struct Pseudodesc idt_pd;

void trap_init(void);
void trap_init_percpu(void);
void print_regs(struct PushRegs *regs);
void print_trapframe(struct Trapframe *tf);
void page_fault_handler(struct Trapframe *tf);
void breakpoint_handler(struct Trapframe *tf);
void backtrace(struct Trapframe *);

void divide_error_trap(void);
void debug_trap(void);
void nmi_trap(void);
void breakpoint_trap(void);
void overflow_trap(void);
void bound_trap(void);
void invalid_opcode_trap(void);
void no_device_trap(void);
void double_fault_trap(void);
void invalid_tss_trap(void);
void seg_not_present_trap(void);
void stack_trap(void);
void gp_trap(void);
void page_fault_trap(void);
void floating_point_trap(void);
void align_check_trap(void);
void machine_check_trap(void);
void simd_trap(void);
void syscall_trap(void);

void timer_trap(void);
void kbd_trap(void);
void irq2_trap(void);
void irq3_trap(void);
void serial_trap(void);
void irq5_trap(void);
void irq6_trap(void);
void spurious_trap(void);
void irq8_trap(void);
void irq9_trap(void);
void irq10_trap(void);
void irq11_trap(void);
void irq12_trap(void);
void irq13_trap(void);
void ide_trap(void);
void irq15_trap(void);
#endif /* JOS_KERN_TRAP_H */

