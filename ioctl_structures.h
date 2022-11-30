#ifndef IOCTL_STRUCTURES
#define IOCTL_STRUCTURES

struct ioctl_pci_dev {
    unsigned int devfn;
    unsigned short vendor;
    unsigned short device;
    unsigned int clas;
    unsigned char revision;
    unsigned char hdr_type;
};

struct ioctl_thread_struct {
    unsigned long 	sp; /* [GR1 ] kernel stack pointer */
};

struct pci_parameters {
    struct ioctl_pci_dev* write_pointer;
    uint16_t major;
    uint16_t minor;
};

struct thread_parameters {
    struct ioctl_thread_struct* write_pointer;
    uint16_t pid;
};

#endif