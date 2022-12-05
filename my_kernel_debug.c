#include <linux/kernel.h> 
#include <linux/module.h>
#include <linux/init.h>

#include <linux/ioctl.h>
#include <linux/fs.h> //for file operations
#include <linux/export.h> //for file operations
#include <linux/pci.h>
#include <linux/pid.h>
#include <linux/processor.h>
#include <linux/uaccess.h> //for user copy 

#include "ioctl_structures.h"

#define DEVICE_NAME "ana_device"
#define SUCCESS 0
#define ERROR -1

long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct thread_parameters thread_params;
    struct task_struct* task;
    struct thread_struct thread;
    struct ioctl_thread_struct ret_thread;

    struct pci_parameters pci_params;
    struct pci_dev* pci_dev;
    struct ioctl_pci_dev ret_pci;
    switch (cmd) {
        case IOCTL_GET_THREADSTRUCT:
            if (copy_from_user(&thread_params, (struct thread_parameters*) arg, sizeof(struct thread_parameters)) != 0 ) {
                pr_err("Failed to read thread parameters\n");
                return ERROR;
            }

            printk("Got PID %d", thread_params.pid);
            task = get_pid_task(find_get_pid(thread_params.pid), PIDTYPE_PID);
            if (task == NULL) {
                pr_err("Failed to read thread with PID %d\n", thread_params.pid);
                return ERROR;
            }
            thread = task->thread;
            ret_thread.es = thread.es;
            ret_thread.ds = thread.ds;
            ret_thread.fsindex = thread.fsindex;
            ret_thread.gsindex = thread.gsindex;
            ret_thread.fsbase = thread.fsbase;
            ret_thread.gsbase = thread.gsbase;
            ret_thread.sp = thread.sp;
            copy_to_user(thread_params.write_pointer, &ret_thread, sizeof(struct ioctl_thread_struct));
            break;
        case IOCTL_GET_PCIDEV:
            if (copy_from_user(&pci_params, (struct pci_parameters*) arg, sizeof(struct pci_parameters)) != 0 ) {
                pr_err("Failed to read PCI parameters structure\n");
                return ERROR;
            }

            pci_dev = pci_get_device(pci_params.vendor, pci_params.device, NULL);
            if (pci_dev == NULL) {
                pr_err("Failed to read PCI with vendor ID %d and device ID %d\n", pci_params.vendor, pci_params.device);
                return ERROR;
            }
            ret_pci.devfn = pci_dev->devfn;
            ret_pci.device = pci_dev->device;
            ret_pci.hdr_type = pci_dev->hdr_type;
            ret_pci.revision = pci_dev->revision;
            ret_pci.vendor = pci_dev->vendor;
            ret_pci.clas = pci_dev->class;
            copy_to_user(pci_params.write_pointer, &ret_pci, sizeof(struct ioctl_pci_dev));
            break;
        default:
            return ERROR;
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
        pr_err("Failed to register the character device %s, return code %d\n", DEVICE_NAME, ret_val);
        return ret_val;
    }

    pr_info("Successfully registered the character device %s with major number %d\n", DEVICE_NAME, ANA_IOC_MAGIC); 
    return SUCCESS;
}
  
static void ana_device_exit(void) {
    unregister_chrdev(ANA_IOC_MAGIC, DEVICE_NAME);
    printk("Successfully unregistered the character device %s\n", DEVICE_NAME);
}


module_init(ana_device_init);
module_exit(ana_device_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module for sending debug info about task_struct");
MODULE_VERSION("1.0");
 