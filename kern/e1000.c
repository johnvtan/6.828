#include <inc/assert.h>
#include <kern/pci.h>
#include <kern/e1000.h>
#include <kern/pmap.h>

#define E1000_STATUS_INIT (0x80080783)

uint32_t e1000_status(void);
static uint32_t mmio_read(volatile uint32_t *base, uint32_t off);

// global pointer to e1000 mmio region in kernel VM
static volatile uint32_t *e1000_mmio_base;

// LAB 6: Your driver code here
int
pci_e1000_attach(struct pci_func *pcif) {
    pci_func_enable(pcif);

    // after calling pci_func_enable, pcif contains info for BAR0 and BAR1 
    // BAR0 contains the base address and size of the MMIO region for the E1000
    // so we need to map it into kernel's VM
    e1000_mmio_base = (uint32_t*)mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
    assert(e1000_status() == E1000_STATUS_INIT);
    cprintf("PCI e1000 attached successfully\n");
    return 0;
}

/*
 * Function to read the status register of the E1000.
 */
uint32_t
e1000_status(void) {
    return mmio_read(e1000_mmio_base, E1000_STATUS);  
}

/*
 * Helper to read some memory address given the base and offset
 * The offset is given in bytes, but since we're adding it to a uint32_t,
 * the address increments by 4 bytes, so we must divide the offset by 4 to 
 * get the correct address.
 */
static uint32_t
mmio_read(volatile uint32_t *base, uint32_t off) {
    return *(base + (off / 4));
}

/*
 * Helper to write a 32 bit value to a memory address described by its base
 * and offset.
 */
static void
mmio_write(volatile uint32_t *base, uint32_t off, uint32_t val) {
    *(base + (off / 4)) = val;
}
