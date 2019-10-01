#include <inc/assert.h>
#include <kern/pci.h>
#include <kern/e1000.h>
#include <kern/pmap.h>

#define E1000_STATUS_INIT (0x80080783)

uint32_t e1000_status(void);
static uint32_t mmio_read(volatile uint32_t *base, uint32_t off);
static void mmio_write(volatile uint32_t *base, uint32_t off, uint32_t val);

// global pointer to e1000 mmio region in kernel VM
static volatile uint32_t *e1000_mmio_base;

/*
 * Initializes the e1000 device. Allocates the MMIO region and maps it 
 * into kernel VM
 */
int
pci_e1000_attach(struct pci_func *pcif) {
    pci_func_enable(pcif);

    // after calling pci_func_enable, pcif contains info for BAR0 and BAR1 
    // BAR0 contains the base address and size of the MMIO region for the E1000
    // so we need to map it into kernel's VM
    e1000_mmio_base = (uint32_t*)mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);

    // Sanity check the status reg's initialized value
    assert(e1000_status() == E1000_STATUS_INIT);

    // Tx initialization sequence
    // 1. Allocate region of memory for Tx descriptor list. Should be 16 byte aligned (64 bit aligned?)
    // 2. Set the Tx Descriptor Base address reg (TDBAL) with the allocated address
    // 3. Set the TDLEN reg. Should be 128 byte aligned.
    // 4. Write 0b to TDH and TDT.
    // 5. Initialize Tx CTL reg
    //      - Set EN bit
    //      - Set PSP bit to 1
    //      - Configure Collision Threshold
    //      - Configure Collision Distance
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
