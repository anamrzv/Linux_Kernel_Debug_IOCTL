#include <linux/kernel.h> 
#include <linux/module.h>

#include <linux/ioctl.h>
#include <linux/fs.h> //for file operations
#include <linux/export.h> //for file operations
#include <linux/pci.h>
#include <linux/pid.h>
#include <linux/processor.h>
#include <linux/uaccess.h> //for user copy 

#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/slab.h>


#include "ioctl_structures.h"

#define ANA_IOC_MAGIC '\x45'

#define IOCTL_GET_THREADSTRUCT _IOR(ANA_IOC_MAGIC, 0, struct thread_struct)
#define IOCTL_GET_PCIDEV _IOR(ANA_IOC_MAGIC, 1, struct pci_dev)

#define DEVICE_NAME "ana_device"
#define SUCCESS 0

long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct thread_parameters thread_params = {0};
    struct task_struct* task = {0};
    struct thread_struct thread = {0};
    struct ioctl_thread_struct ret_thread = {0};

    struct pci_parameters pci_params = {0};
    struct pci_dev* pci_dev = {0};
    struct ioctl_pci_dev ret_pci = {0};
    switch (cmd) {
        case IOCTL_GET_THREADSTRUCT:
            if (copy_from_user(&thread_params, (struct thread_parameters*) arg, sizeof(struct thread_parameters)) != 0 ) {
                pr_alert("Failed to read thread parameters\n");
                return -1;
            }

            pr_info("Got PID %d", thread_params.pid);
            task = get_pid_task(find_get_pid(thread_params.pid), PIDTYPE_PID);
            if (task == NULL) {
                pr_alert("Failed to read thread with PID %d\n", thread_params.pid);
                return -1;
            }
            thread = task->thread;
            // ret_thread.esp0 = thread.esp0;
            // ret_thread.fp = thread.fp;
            // ret_thread.ksp = thread.ksp;
            // ret_thread.lr = thread.lr;
            // ret_thread.pc = thread.pc;
            // ret_thread.seqstat = thread.seqstat;
            // ret_thread.single_step_addr = thread.single_step_addr;
            ret_thread.sp = thread.sp;
            // ret_thread.usp = thread.usp;
            // ret_thread.wchan = thread.wchan;
            copy_to_user(thread_params.write_pointer, &ret_thread, sizeof(struct ioctl_thread_struct));
            break;
        case IOCTL_GET_PCIDEV:
            if (copy_from_user(&pci_params, (struct pci_parameters*) arg, sizeof(struct pci_parameters)) != 0 ) {
                pr_alert("Failed to read pci parameters\n");
                return -1;
            }

            pci_dev = pci_get_device(pci_params.major, pci_params.minor, pci_dev);
            if (pci_dev == NULL) {
                pr_alert("Failed to read PCI with major %d and minor %d\n", pci_params.major, pci_params.minor);
                return -1;
            }
            ret_pci.devfn = pci_dev->devfn;
            ret_pci.device = pci_dev->device;
            ret_pci.hdr_type = pci_dev->hdr_type;
            ret_pci.revision = pci_dev->revision;
            ret_pci.vendor = pci_dev->vendor;
            ret_pci.clas = pci_dev->class;
            copy_to_user(pci_params.write_pointer, &ret_pci, sizeof(struct ioctl_pci_dev));
            
            break;
    }
    return SUCCESS;
}

static struct file_operations fops = {
	.unlocked_ioctl = device_ioctl,
	.owner = THIS_MODULE, //macro: pointer to the module structure of this module
};

static int ana_device_init(void) {
    int ret_val;
    pr_info("Ana Debug Module is initializing...\n");
    ret_val = register_chrdev(ANA_IOC_MAGIC, DEVICE_NAME, &fops);
    if (ret_val < 0) {
        pr_alert("Failed to register the character device %s, return code %d\n", DEVICE_NAME, ret_val);
        return ret_val;
    }

    pr_info("Successfully registered the character device %s with major number %d\n", DEVICE_NAME, ANA_IOC_MAGIC);
 
    return SUCCESS;
}
 
static void ana_device_exit(void) {
    unregister_chrdev(ANA_IOC_MAGIC, DEVICE_NAME);
    pr_alert("Successfully unregistered the character device %s\n", DEVICE_NAME);
}


module_init(ana_device_init);
module_exit(ana_device_exit);

MODULE_LICENSE("GPL"); //TODO:
MODULE_DESCRIPTION("Module for sending debug info about task_struct");
MODULE_VERSION("1.0");
 