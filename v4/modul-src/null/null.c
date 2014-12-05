#include <linux/init.h>
#include <linux/module.h>
/* prototypes */
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "null"

/* normaly in stdlib.h */
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

static int major;
static struct file_operations fobs;
static dev_t dev_number;
static struct cdev *driver_object;
struct class *null_class;

static char driver_buffer[1024];

static int driver_open(struct inode *geraetedatei, struct file *instanz);
static int driver_release(struct inode *geraetedatei, struct file *instanz);

static ssize_t driver_write(struct file *instanz, const char *user, size_t count, loff_t *offset);


static struct file_operations fobs =
{
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_release,
	.write = driver_write
};

static int driver_open(struct inode *geraetedatei, struct file *instanz)
{
	printk(KERN_INFO "NULL Driver open!\n");
	return EXIT_SUCCESS;
}

static int driver_release(struct inode *geraetedatei, struct file *instanz)
{
	printk(KERN_INFO "Release driver!\n");
	return EXIT_SUCCESS;
}

static ssize_t driver_write(struct file *instanz, const char *user, size_t count, loff_t *offset)
{
/*	size_t to_copy = count;
	
	printk(KERN_INFO "NULL write called...\n");
	
	if(to_copy > sizeof(driver_buffer)) {
		to_copy = sizeof(driver_buffer);
	}
	
	if(copy_from_user(driver_buffer, user, to_copy) != 0) {
		return -EFAULT;
	}
	printk(KERN_INFO "Read %s from user... byebye!", driver_buffer);
	return to_copy;
 */
	printk("NULL: Write\n");
	printk("%d Bytes\n", count);
	return count;
}

static int __init ModInit(void)
{
	/* reserve device driver number */
	if(alloc_chrdev_region(&dev_number, 0, 1, DRIVER_NAME) < 0)
	{
		printk("failed to alloc_chrdev_region\n");	
		return -EIO;
	}
	/* reserve object */
	driver_object = cdev_alloc();
	
	if(driver_object == NULL)
	{
		goto free_device_number;
	}
	driver_object->owner = THIS_MODULE;
	driver_object->ops = &fobs;
	
	if(cdev_add(driver_object, dev_number, 1))
	{
		goto free_cdev;
	}
	
	null_class = class_create(THIS_MODULE, DRIVER_NAME);
	device_create(null_class, NULL, dev_number, NULL, "%s", DRIVER_NAME);
	
	major = MAJOR(dev_number);
	
	printk("Major number: %d\n", major);
	return EXIT_SUCCESS;
	
free_cdev:
	kobject_put(&driver_object->kobj);
	
free_device_number:
	unregister_chrdev_region(dev_number, 1);
	return -EIO;
	
}

static void __exit ModExit(void) 
{
	device_destroy(null_class, dev_number);
	class_destroy(null_class);
	cdev_del(driver_object);
	unregister_chrdev_region(dev_number, 1);
	return;
}

module_init(ModInit);
module_exit(ModExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Timotheus Ruprecht and  Steffen Gorenflo");
MODULE_DESCRIPTION("Modul Null");
MODULE_SUPPORTED_DEVICE("none");


