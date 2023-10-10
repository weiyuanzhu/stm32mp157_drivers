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
#include <linux/kern_levels.h>

static struct class *firstdrv_class;
static struct device *firstdrv_class_dev;
static struct device *firstdrv_class_dev2;

volatile unsigned long *gpfcon = NULL;
volatile unsigned long *gpfdat = NULL;

static int first_drv_open(struct inode *inode, struct file *file)
{
    // printk(KERN_INFO "first_drv_open()\n\r");
    //  config GPF 4, 5, 6; 01 = Output
    *gpfcon &= ~((0x3 << (4 * 2)) | (0x3 << (5 * 2)) | (0x3 << (6 * 2)));
    *gpfcon |= ((0x01 << (4 * 2)) | (0x01 << (5 * 2)) | (0x01 << (6 * 2)));

    return 0;
}

static ssize_t first_drv_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    int val;
    // printk(KERN_INFO "first_drv_write()\n\r");

    copy_from_user(&val, buf, count); // copy_to_user();
    printk(KERN_INFO "first_drv_write() val:%d\n\r", val);
    if (val == 1)
    {
        // turn on LEDs
        *gpfdat &= ~((1 << 4) | (1 << 5) | (1 << 6));
    }
    else
    {
        // turn off LEDs
        *gpfdat |= ((1 << 4) | (1 << 5) | (1 << 6));
    }

    printk(KERN_INFO "first_drv_write() gpfdata:%d\n\r", *gpfdat);

    return 0;
}

static struct file_operations first_drv_fops = {
    .owner = THIS_MODULE, /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
    .open = first_drv_open,
    .write = first_drv_write,
};

int major;
static int __init first_drv_init(void)
{
    printk(KERN_INFO "[ KERN_EMERG ]  First Driver Module Init\n");
    major = register_chrdev(0, "first_drv", &first_drv_fops); // register driver
    printk(KERN_INFO "register_chrdev, major: %d, return code:%d\n", major, major);

    firstdrv_class = class_create(THIS_MODULE, "firstdrv");
    firstdrv_class_dev = device_create(firstdrv_class, NULL, MKDEV(major, 0), NULL, "xyz");      /* /dev/xyz */
    firstdrv_class_dev2 = device_create(firstdrv_class, NULL, MKDEV(major, 1), NULL, "xyz1"); /* /dev/xyz1 */

    // re-map GPFCON and GPFDATA
    gpfcon = (volatile unsigned long *)ioremap(0x50002000, 32);
    // gpfdat = (volatile unsigned long *)ioremap(0x56000054, 16);
    gpfdat = gpfcon + 1; // pointer + 1 实际增加unsigned long的长度

    return major;
}

static void __exit first_drv_exit(void)
{
    device_destroy(firstdrv_class, MKDEV(major, 0));
    device_destroy(firstdrv_class, MKDEV(major, 1));
    class_destroy(firstdrv_class);
    unregister_chrdev(major, "first_drv"); // unregister driver
    // unmap IO
    iounmap(gpfcon);
    iounmap(gpfdat);

    printk(KERN_INFO "first_drv exited\n");
}

module_init(first_drv_init);
module_exit(first_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("WZ");
MODULE_DESCRIPTION("first drive module");
MODULE_ALIAS("test_module");
