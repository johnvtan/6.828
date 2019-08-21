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

