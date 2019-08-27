# Exercise 2
## Question 1
MBOOTPHYS is needed because `kern/mpentry.S` is linked to run above KERNBASE, but since the AP runs in real mode when it
starts up, it cannot access high memory. The BSP copies the code from `kern/mpentry.S` to address 0x7000, but memory
references in the code would still refer to addresses in high memory because it is linked to run above KERNBASE. Symbols
must therefore be manually calculated using the MBOOTPHYS in order to account for the difference in the entry code's
physical location and its link address.
