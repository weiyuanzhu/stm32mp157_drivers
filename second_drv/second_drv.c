#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/export.h>
#include <linux/atomic.h>
#include <asm/atomic.h>
#include <asm/io.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/device.h>

static int major;
static struct class *second_drv_class;
static struct class_device *second_drv_class_dev;

volatile unsigned long *gpfcon = NULL;
volatile unsigned long *gpfdat = NULL;

volatile unsigned long *gpgcon = NULL;
volatile unsigned long *gpgdat = NULL;

static int second_drv_open(struct inode *inode, struct file *file)
{
    *gpfcon &= ~((0x03 << (0 * 2)) | (0x03 << (2 * 2)));  // GPF0 GPF2 input - 00
    *gpgcon &= ~((0x03 << (3 * 2)) | (0x03 << (11 * 2))); // GPF3 GPF11 input - 00
    return 0;
}

static ssize_t second_drv_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
    char key_vals[4];
    key_vals[0] = (*gpfdat & (1 << 0)) ? 1 : 0;
    key_vals[1] = (*gpfdat & (1 << 2)) ? 1 : 0;
    key_vals[2] = (*gpgdat & (1 << 3)) ? 1 : 0;
    key_vals[3] = (*gpgdat & (1 << 11)) ? 1 : 0;

    copy_to_user(buf, key_vals, sizeof(key_vals));

    return sizeof(key_vals);
}

static ssize_t second_drv_write(struct file *file, const char __user *buf, size_t size, loff_t *ppos)
{
    return 0;
}

static struct file_operations second_drv_fops = {
    .owner = THIS_MODULE,
    .read = second_drv_read,
    .open = second_drv_open,
    .write = second_drv_write,
};

static int __init second_drv_init(void)
{
    major = register_chrdev(0, "second_drv", &second_drv_fops);
    second_drv_class = class_create(THIS_MODULE, "second_drv");
    second_drv_class_dev = device_create(second_drv_class, NULL, MKDEV(major, 0), NULL, "buttons"); /* /dev/buttons */

    gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
    gpfdat = (volatile unsigned long *)ioremap(0x56000054, 16);

    gpgcon = (volatile unsigned long *)ioremap(0x56000060, 16);
    gpgdat = (volatile unsigned long *)ioremap(0x56000064, 16);

    return major;
}

static int __exit second_drv_exit(void)
{
    unregister_chrdev(major, "second_drv");
    device_unregister(second_drv_class_dev);
    class_destroy(second_drv_class);
    iounmap(gpfcon);
    iounmap(gpfdat);
    iounmap(gpgcon);
    iounmap(gpgdat);
    return 0;
}

module_init(second_drv_init);
module_exit(second_drv_exit);
MODULE_LICENSE("GPL");