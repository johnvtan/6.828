# Exercise 2
## Question 1
MBOOTPHYS is needed because `kern/mpentry.S` is linked to run above KERNBASE, but since the AP runs in real mode when it
starts up, it cannot access high memory. The BSP copies the code from `kern/mpentry.S` to address 0x7000, but memory
references in the code would still refer to addresses in high memory because it is linked to run above KERNBASE. Symbols
must therefore be manually calculated using the MBOOTPHYS in order to account for the difference in the entry code's
physical location and its link address.

# Exercise 5
## Question 1
If a user program calls a syscall, the kernel is only locked after hardware pushes values on the stack. If multiple
programs running on different processors issue a syscall at the same time and the kernel only used a single stack, all
the values would be mixed together and neither processor would be able to return to user mode correctly. 

# Exercise 6
## Question 1
Kernel memory is mapped to the top of each environment's address space, so even after the page directory is changed, we
can still dereference `e`.

## Question 2
An environment should be able to continue to run where it left off after returning from kernel mode. This is especially
relevant when preemptive interrupts are implemented. User context is saved on a kernel stack when in the trap handler.
