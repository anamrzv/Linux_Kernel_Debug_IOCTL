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
    unsigned long sp;
    unsigned short es;
	unsigned short ds;
	unsigned short fsindex;
	unsigned short gsindex;
    unsigned long fsbase;
	unsigned long gsbase;
};

struct pci_parameters {
    struct ioctl_pci_dev* write_pointer;
    uint32_t vendor;
    uint32_t device;
};

struct thread_parameters {
    struct ioctl_thread_struct* write_pointer;
    uint32_t pid;
};

#endif