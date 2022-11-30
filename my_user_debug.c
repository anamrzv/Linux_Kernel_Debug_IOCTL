#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <inttypes.h>
#include "ioctl_structures.h"

#define ANA_IOC_MAGIC '\x45'
#define PCI_DEV_OPTION 1
#define THREAD_STRUCT_OPTION 2
#define DEVICE_FILE "/dev/ana_device"

#define IOCTL_GET_THREADSTRUCT _IOR(ANA_IOC_MAGIC, 0, struct thread_parameters*)
#define IOCTL_GET_PCIDEV _IOR(ANA_IOC_MAGIC, 1, struct pci_parameters*)

void print_pci(struct ioctl_pci_dev* pci) {
    printf("devfn %d\n", pci->devfn);
    printf("vendor %d\n", pci->vendor);
    printf("device %d\n", pci->device);
    printf("class %d\n", pci->clas);
    printf("revision %d\n", pci->revision );
    printf("hdr_type %d\n", pci->hdr_type);
}

void print_thread(struct ioctl_thread_struct* th) {
    // printf("single_step_addr %d\n", th->single_step_addr);
    // printf("kernel stack pointer %d\n", th->ksp);
    // printf("saved status register %d\n", th->seqstat);
    // printf("SR of stack frame pt_regs %d\n", th->esp0);
    // printf("user stack pointer %d\n", th->usp);
    // printf("instruction pointer %d\n", th->pc);
    // printf("wchan %D\n", th->wchan);
    printf("[GR1 ] kernel stack pointer %ld\n", th->sp);
    // printf("[GR2 ] kernel frame pointer %d\n", th->fp);
    // printf("link register %d", th->lr);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Programm needs at least 1 argument\n");
        return -1;
    }

    int option;

    if (argc == 4 && strcmp((char*) argv[1], "pci_dev") == 0) option = PCI_DEV_OPTION;
    else if (argc == 3 && strcmp((char*) argv[1], "thread_struct") == 0) option = THREAD_STRUCT_OPTION;
    else {
        printf("Please check that your argument is either 'pci_dev <major number> <minor number>' or 'thread_struct <thread PID>'\n");
        return -1;
    }
    
    printf("Trying to request kernel for debug info about structure: %s\n", (char*) argv[1]);
    
    int fd = open(DEVICE_FILE, O_WRONLY);
    if (fd < 0) {
        printf("Couldn't open device file %s, check if it exists in /dev\n", DEVICE_FILE);
        return -1;
    }

    printf(fd);

    if (option == PCI_DEV_OPTION) {
        struct ioctl_pci_dev pci_dev = {0};
        struct pci_parameters pci_params = { &pci_dev, (uint16_t) argv[2], (uint16_t) argv[3] };
        uint8_t ret = ioctl(fd, IOCTL_GET_PCIDEV, &pci_params);
        printf("%d", ret);
        if (ret == 0) print_pci(&pci_dev);
    } else if (option == THREAD_STRUCT_OPTION) {
        struct ioctl_thread_struct thread = {0};
        struct thread_parameters thread_params = { &thread, (uint16_t) argv[2] };
        uint8_t ret = ioctl(fd, IOCTL_GET_THREADSTRUCT, &thread_params);
        printf("%d", ret);
        if (ret == 0) print_thread(&thread);
    }

}