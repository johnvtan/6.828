#include <inc/assert.h>
#include <kern/pci.h>
#include <kern/e1000.h>
#include <kern/pmap.h>

// LAB 6: Your driver code here
int
pci_e1000_attach(struct pci_func *pcif) {
    cprintf("pci_e1000_attach called\n");
    pci_func_enable(pcif);
    return 0;
}
