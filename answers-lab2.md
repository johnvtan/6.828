# Question 1
Assuming that the following JOS kernel code is correct, what type should variable x have, `uintptr_t` or `physaddr_t`?

```C
   mystery_t x;
   char* value = return_a_pointer();
   *value = 10;
   x = (mystery_t) value;
```

`x` should be of type `uintptr_t`, since `uintptr_t` refers to virtual memory pointers. The kernel was initialized to
map virtual addresses `[KERNBASE, KERNBASE + 4MB]` to `[0, 4MB]` in `entry.S`. By default, the kernel will use addresses
in kernel virtual memory unless explicitly converted to a physical address. However, the kernel can still access
physical memory directly if it needs to since `[0, 4MB]` was identity mapped in the intial page table.

# Exercise 5
## Question 1
Entry          Virtual Address              Points to
956            0xef000000                   Page Info array (Read only)
957            0xef400000                   Virtual page table (at UVPT)
958            0xef800000                   Unmapped (stack guard page)
959            0xefc00000                   Kernel stack
960 and above  0xf0000000                   Maps [0, 0xFFFFFFFF-KERNBASE) to
                                            [KERNBASE, 0xFFFFFFFF)

## Question 2
User programs can't access kernel memory since the MMU checks the permission bits in each page directory and page table
entry before actually accessing a physical memory location. All the kernel pages are readable and writeable by the
kernel but cannot be accessed by unprivileged programs because the page table entries for those pages have the User bit
set to 0. If a user program attempts to access a page whose User bit is 0, the MMU will refuse to access the memory and
will instead trigger a page fault.

## Question 3
The operating system reserves all memory above 0xF0000000 for itself. Since the maximum address of a 32 bit address
space is 0xFFFFFFFF, the OS can be no larger than (0xFFFFFFFF - 0xF0000000) bytes, or ~268MB.

## Question 4
With 268MB=0x1000000 of RAM reserved for the OS split into 4096=0x1000 byte pages, we get 0x1000000/0x1000 = 0x10000
or 65536 page frames. We would need an array of PageInfo structs, which are each 8 bytes. We would also need a page
entry for each of the page frames, each of which occupy 4 bytes. Finally, we would also need to allocate a page
directory which itself is 4096 bytes. So in total, we would need (8+4)*65536 + 4096 = 790528 bytes.

## Question 5
The code in entry.S continues using a low memory address. We transition to addresses above KERNBASE when we jump to C
code, since that code is linked to a high memory address - in kernel.ld, it is specified that the kernel begins at
address 0xF0100000.
