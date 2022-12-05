#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <inttypes.h>
#include "ioctl_structures.h"

#define PCI_DEV_OPTION 1
#define THREAD_STRUCT_OPTION 2
#define DEVICE_FILE "/dev/ana_device"

void print_pci(const struct ioctl_pci_dev* pci) {
    printf("Vendor ID %hu\n", pci->vendor);
    printf("Device ID %hu\n", pci->device);
    printf("Encoded device & function index %u\n", pci->devfn);
    printf("Class Code %u\n", pci->clas);
    printf("Revision ID %u\n", pci->revision);
    printf("Header Type %u\n", pci->hdr_type);
}

void print_thread(const struct ioctl_thread_struct* th) {
    printf("es %hu\n", th->es);
    printf("ds %hu\n", th->ds);
    printf("fsindex %hu\n", th->fsindex);
    printf("gsindex %hu\n", th->gsindex);
    printf("Frame base %lu\n", th->fsbase);
    printf("gsbase %lu\n", th->gsbase);
    printf("[GR1] kernel stack pointer %lu\n", th->sp);
}

void init_pci_params(struct ioctl_pci_dev* ptr, const char* v, const char* d, struct pci_parameters* target) {
    struct pci_parameters* pci_params = malloc(sizeof(struct pci_parameters));
    pci_params->device = strtoul(v, NULL, 16);
    pci_params->vendor = strtoul(d, NULL, 16);
    pci_params->write_pointer = ptr;
    target = pci_params;
}

void init_thread_params(struct ioctl_thread_struct* ptr, const char* pid, struct thread_parameters* target) {
    struct thread_parameters* thread_params = malloc(sizeof(struct thread_parameters));
    thread_params->write_pointer = ptr;
    thread_params->pid = strtoul(pid, NULL, 10);
    target = thread_params;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Programm needs at least 1 argument\n");
        return -1;
    }

    int option;

    if (argc == 4 && strcmp(argv[1], "pci_dev") == 0) option = PCI_DEV_OPTION;
    else if (argc == 3 && strcmp(argv[1], "thread_struct") == 0) option = THREAD_STRUCT_OPTION;
    else {
        printf("Please check that your argument is either 'pci_dev <major number hex> <minor number hex>' or 'thread_struct <thread PID>'\n");
        return -1;
    }
    
    printf("Trying to request kernel for debug info about structure: %s\n", argv[1]);
    
    int fd = open(DEVICE_FILE, O_WRONLY);
    if (fd < 0) {
        printf("Couldn't open device file %s, check if it exists in /dev\n", DEVICE_FILE);
        return -1;
    }

    struct pci_parameters* pci_params;
    struct thread_parameters* thread_params;

    struct ioctl_pci_dev pci_dev;
    struct ioctl_thread_struct thread;

    switch (option) {
        case PCI_DEV_OPTION:
            init_pci_params(&pci_dev, argv[2], argv[3], pci_params);
            if (ioctl(fd, IOCTL_GET_PCIDEV, &pci_params)) print_pci(&pci_dev);
            else printf("Failed to get info from kernel. Check if the parameters are right.\n");
            free(pci_params->write_pointer);
            free(pci_params);
            break;
        case THREAD_STRUCT_OPTION:
            init_thread_params(&thread, argv[2], thread_params);
            if (ioctl(fd, IOCTL_GET_THREADSTRUCT, &thread_params)) print_thread(&thread);
            else printf("Failed to get info from kernel. Check if the parameters are right.\n");
            free(thread_params->write_pointer);
            free(thread_params);
            break;
        default:
            printf("No such option\n");
            break;
    }

    return 0;
}