// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t utf_err = utf->utf_err;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.
    if (!(utf_err & FEC_WR)) {
        panic("Page fault on a read??? - va = %08x\n", (uint32_t)addr);
    }

    int pn = PGNUM(addr); 

    if (!(uvpt[pn] & PTE_COW)) {
        panic("Page fault on a page that isn't COW - va = %08x\n", (uint32_t)addr);
    }

	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
    int perm = PTE_W | PTE_P | PTE_U;
    int err = sys_page_alloc(0, (void*)PFTEMP, perm);
    if (err < 0) {
        panic("pgfault: sys_page_alloc failed with %e\n", err);
    }

    void* fault_page = ROUNDDOWN(addr, PGSIZE);
    // move the COW page data into PFTEMP page
    memmove((void*)PFTEMP, fault_page, PGSIZE);

    // then remap PFTEMP to fault_page 
    err = sys_page_map(0, (void*)PFTEMP, 0, (void*)fault_page, perm); 
    if (err < 0) {
        panic("pgfault: sys_page_map failed with %e\n", err);
    }

    // then unmap PFTEMP
    err = sys_page_unmap(0, (void*)PFTEMP);
    if (err < 0) {
        panic("pgfault: failed unmapping page with %e\n", err);
    }
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
    void *va = (void*)(pn * PGSIZE);
    int perm = PTE_U | PTE_P;
    int err;

    // only copy page mapping if shared
    if (uvpt[pn] & PTE_SHARE) {
        err = sys_page_map(0, va, envid, va, uvpt[pn] & PTE_SYSCALL);
        if (err < 0) {
            panic("duppage: Failed trying to sys_page_map (PTE_SHARE)");
        }
        return 0;
    }

    // here, page isn't shared
    if (uvpt[pn] & PTE_W || uvpt[pn] & PTE_COW) {
        perm |= PTE_COW;
    }

    err = sys_page_map(0, va, envid, va, perm);
    if (err < 0) {
        panic("duppage: Failed trying to sys_page_map - %e\n", err);
    }

    if (perm & PTE_COW) {
        err = sys_page_map(0, va, 0, va, perm); 
        if (err < 0) {
            panic("duppage: Failed trying to remap page as COW - %e\n", err);
        }
    }
	return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
    // set up page fault handler
    set_pgfault_handler(pgfault);

    // create child
    envid_t child_id = sys_exofork();

    if (child_id < 0) {
        panic("fork: Error on sys_exofork() - returned %e\n", child_id);
    }

    if (child_id == 0) {
        // child code
        thisenv = &envs[ENVX(sys_getenvid())];
        return 0;
    }

    // parent code
    // duplicate pages from parent to child
    uintptr_t addr;
    for (addr = 0; addr < (UXSTACKTOP - PGSIZE); addr += PGSIZE) {
        if (PT_PRESENT(uvpd[PDX(addr)]) && PT_PRESENT(uvpt[PGNUM(addr)])) {
            duppage(child_id, PGNUM(addr));
        }
    } 

    // create new exception stack for child
    int err = sys_page_alloc(child_id, (void*)(UXSTACKTOP - PGSIZE), PTE_U | PTE_W | PTE_P);  
    if (err < 0) {
        panic("fork: Failed to allocate exception stack for child: %e\n", err);
    }

    // set page fault handler
    err = sys_env_set_pgfault_upcall(child_id, thisenv->env_pgfault_upcall);
    if (err < 0) {
        panic("fork: Couldn't set child's pgfault handler: %e\n");
    }

    // mark child as runnable
    err = sys_env_set_status(child_id, ENV_RUNNABLE);
    if (err < 0) {
        panic("fork: Failed to mark child process as runnable: %e\n", err);
    }

    return child_id;
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
