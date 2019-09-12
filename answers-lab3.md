# Exercise 4
## Question 1
There must be a handler for each exception/interrupt because there's no other way to differentiate between which
interrupt occurred. The exception number isn't pushed onto the stack by the hw, so it would be difficult for a single generic
handler to differentiate between e.g., a page fault and double fault where the exceptions require very different
responses. This is solved by the trap handling mechanism in JOS, since we manually push the trap number onto the stack.

## Question 2
Because `softint` calls `int $14` from user mode, it will trigger a general protection fault since the interrupt gate
for `int $14` was configured so that only code running with a privilege level of 0 (e.g., the kernel) could call it
directly. Because user mode runs with a privilege level of 3, the MMU triggered a general protection fault and prevented
the user code from running an illegal instruction.

# Exercise 6
## Question 1
If the breakpoint exception's privilege level is set incorrectly to 0, then a user program calling `int $3` won't have a
high enough privilege level to execute the code. The MMU will catch this and signal a general protection fault instead
of calling the breakpoint exception handler.

## Question 2
The purpose of this mechanism is to prevent malicious user programs from accessing potentially dangerous code.
`user/softint` for example tries to run the page fault handler which might result in a user program being able to mess
with its own or another process's page table.
