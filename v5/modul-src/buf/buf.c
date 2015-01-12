#include <linux/init.h>
#include <linux/module.h>
/* prototypes */
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DRIVER_NAME "buf"

/* normaly in stdlib.h */
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/* define makro for atomic read */
#define READ_POSSIBLE (atomic_read(&bytes_available)!=0)

static int major;
static dev_t dev_number;
static struct cdev *driver_object;
struct class *buf_class;


static int driver_open(struct inode *geraetedatei, struct file *instanz);
static int driver_close(struct inode *geraetedatei, struct file *instanz);
static ssize_t driver_read(struct file *instanz, char __user *userbuffer, size_t count, loff_t *offset);
static ssize_t driver_write(struct file *instanz, const char __user *userbuffer, size_t count, loff_t *offs);

static struct file_operations fobs =
{
	.owner = THIS_MODULE,
	.open = driver_open,
	.read = driver_read,
	.write = driver_write,
	.release = driver_close,
};



static int driver_open(struct inode *geraetedatei, struct file *instanz) {
	printk("Device openend");	
	return 0;
}

static int driver_close(struct inode *geraetedatei, struct file *instanz) {
	printk("Device released");	
	return 0;
}

static ssize_t driver_read(struct file *instanz, char __user *userbuffer, size_t count, loff_t *offset) {
	
	int not_copied, to_copy;
	/*TODO: initialize "data_available"*/

	/* non blocking mode and no data available */
	if (!READ_POSSIBLE && (instanz->f_flags&O_NONBLOCK)) {
		return -EAGAIN;
	}
	
	/* blocking mode*/
	if (wait_event_interruptible(wq_read, READ_POSSIBLE)) {
		return -ERESTARTSYS;
	}
	
	to_copy = min((size_t) atomic_read(&bytes_available), count);
	not_copied = copy_to_user(userbuffer, /*TODO: driver buffer*/, to_copy);
    not_copied=copy_to_user(user,hello_world,to_copy);

	/* TODO: reduce buffer*/
	atomic_sub(to_copy-not_copied, &bytes_available);
	     
	/* return to user application */
    return to_copy-not_copied;
			
}

static ssize_t driver_write(struct file *instanz, const char __user *userbuffer, size_t count, loff_t *offs) {
	
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
	
	buf_class = class_create(THIS_MODULE, DRIVER_NAME);
	device_create(buf_class, NULL, dev_number, NULL, "%s", DRIVER_NAME);
	
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
	device_destroy(buf_class, dev_number);
	class_destroy(buf_class);
	cdev_del(driver_object);
	unregister_chrdev_region(dev_number, 1);
	return;
}

module_init(ModInit);
module_exit(ModExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Timotheus Ruprecht and  Steffen Gorenflo");
MODULE_DESCRIPTION("Modul Template");
MODULE_SUPPORTED_DEVICE("none");


