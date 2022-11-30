#ifndef IOCTL_STRUCTURES
#define IOCTL_STRUCTURES

struct ioctl_pci_dev {
    unsigned int devfn;
    unsigned short vendor;
    unsigned short device;
    unsigned int clas;
    unsigned char revision;
    unsigned char hdr_type;
    const char *driver_name;
};

struct ioctl_thread_struct {
    unsigned long 	single_step_addr;
    unsigned long 	ksp;    /* kernel stack pointer */
    unsigned short 	seqstat; /* saved status register */
    unsigned long 	esp0;   /* points to SR of stack frame pt_regs */
    unsigned long  usp;     /* user stack pointer */
    unsigned long  pc;      /* instruction pointer */
    unsigned long 	wchan;
    unsigned long 	sp; /* [GR1 ] kernel stack pointer */
    unsigned long 	fp; /* [GR2 ] kernel frame pointer */
    unsigned long 	lr; /* link register */
};

struct pci_parameters {
    struct ioctl_pci_dev* write_pointer;
    uint16_t major;
    uint16_t minor;
}

struct thread_parameters {
    struct ioctl_thread_struct* write_pointer;
    uint16_t pid;
}

#endif