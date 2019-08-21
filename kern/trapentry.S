/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text


/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
TRAPHANDLER_NOEC(divide_error_trap, T_DIVIDE)
TRAPHANDLER_NOEC(debug_trap, T_DEBUG)
TRAPHANDLER_NOEC(nmi_trap, T_NMI)
TRAPHANDLER_NOEC(breakpoint_trap, T_BRKPT)
TRAPHANDLER_NOEC(overflow_trap, T_OFLOW)
TRAPHANDLER_NOEC(bound_trap, T_BOUND)
TRAPHANDLER_NOEC(invalid_opcode_trap, T_ILLOP)
TRAPHANDLER_NOEC(no_device_trap, T_DEVICE)
TRAPHANDLER(double_fault_trap, T_DBLFLT)
//TRAPHANDLER_NOEC(coproc_overrun_trap, 
TRAPHANDLER(invalid_tss_trap, T_TSS)
TRAPHANDLER(seg_not_present_trap, T_SEGNP)
TRAPHANDLER(stack_trap, T_STACK)
TRAPHANDLER(gp_trap, T_GPFLT)
TRAPHANDLER(page_fault_trap, T_PGFLT)
//TRAPHANDLER(reserved1_trap, 15)
TRAPHANDLER_NOEC(floating_point_trap, T_FPERR)
TRAPHANDLER(align_check_trap, T_ALIGN)
TRAPHANDLER_NOEC(machine_check_trap, T_MCHK)
TRAPHANDLER_NOEC(simd_trap, T_SIMDERR)

// for syscalls
TRAPHANDLER_NOEC(syscall_trap, T_SYSCALL)

/*
 * Lab 3: Your code here for _alltraps
 */

// I should already be on the kernel stack right?
_alltraps:
    /* 
     * Stack should already have:
     *   - SS
     *   - ESP
     *   - EFLAGS
     *   - CS
     *   - EIP
     *   - err (or 0 if trap doesn't do error codes)
     */

    // push ds, es
    pushl %ds
    pushl %es

    // pushes a bunch of regs
    pushal

    // load GD_KD into ds, es
    movw $GD_KD, %ax
    movw %ax, %es
    movw %ax, %ds

    // pass Trapframe as a parameter to trap
    pushl %esp
    call trap
