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
    printf("Vendor ID %hu\n", pci->vendor);
    printf("Device ID %hu\n", pci->device);
    printf("Encoded device & function index %u\n", pci->devfn);
    printf("Class Code %u\n", pci->clas);
    printf("Revision ID %u\n", pci->revision);
    printf("Header Type %u\n", pci->hdr_type);
}

void print_thread(struct ioctl_thread_struct* th) {
    printf("es %hu\n", th->es);
    printf("ds %hu\n", th->ds);
    printf("fsindex %hu\n", th->fsindex);
    printf("gsindex %hu\n", th->gsindex);
    printf("Frame base %lu\n", th->fsbase);
    printf("gsbase %lu\n", th->gsbase);
    printf("[GR1] kernel stack pointer %lu\n", th->sp);
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
        printf("Please check that your argument is either 'pci_dev <major number hex> <minor number hex>' or 'thread_struct <thread PID>'\n");
        return -1;
    }
    
    printf("Trying to request kernel for debug info about structure: %s\n", (char*) argv[1]);
    
    int fd = open(DEVICE_FILE, O_WRONLY);
    if (fd < 0) {
        printf("Couldn't open device file %s, check if it exists in /dev\n", DEVICE_FILE);
        return -1;
    }

    if (option == PCI_DEV_OPTION) {
        struct ioctl_pci_dev pci_dev = {0};
        uint32_t vendor = strtoul(argv[2], NULL, 16);
        uint32_t device = strtoul(argv[3], NULL, 16);
        struct pci_parameters pci_params = { .write_pointer = &pci_dev, .vendor = vendor, .device = device };
        uint8_t ret = ioctl(fd, IOCTL_GET_PCIDEV, &pci_params);
        if (ret == 0) print_pci(&pci_dev);
        else printf("Failed to get info from kernel. Check if the parameters are right.\n");
    } else if (option == THREAD_STRUCT_OPTION) {
        struct ioctl_thread_struct thread = {0};
        uint32_t pid = strtoul(argv[2], NULL, 10);
        struct thread_parameters thread_params = { .write_pointer = &thread, .pid = pid };
        uint8_t ret = ioctl(fd, IOCTL_GET_THREADSTRUCT, &thread_params);
        if (ret == 0) print_thread(&thread);
        else printf("Failed to get info from kernel. Check if the parameters are right.\n");
    }

    return 0;
}