#include <linux/init.h>
#include <linux/module.h>
/* prototypes */
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

#define DRIVER_NAME "myzero"
#define HELLO "Hello World\n"
#define ZERO "0\n"

/* normaly in stdlib.h */
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define NUMBER_OF_MINOR 2

static int major;
static struct file_operations fobs;
static dev_t dev_number;
static struct cdev *driver_object;
struct class *template_class;

struct myData {
    int available_bytes;
};


static int driver_open(struct inode *geraetedatei, struct file *instanz);
static int driver_release(struct inode *geraetedatei, struct file *instanz);

static ssize_t driver_read(struct file *instanz, char *user, size_t count, loff_t *offset);

static struct file_operations fobs =
{
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_release,
	.read = driver_read,
};

static int driver_open(struct inode *geraetedatei, struct file *instanz)
{		

	struct myData *ptr;
	
	/* 	GFP_KERNEL aufrufende Prozess wird in Zustand wartend versetzt,
	 * 	falls kein Speicher zur Verfuegung steht. */
	ptr = (struct myData*) kmalloc(sizeof(struct myData),GFP_KERNEL);

	if (ptr == 0) {
		printk(KERN_INFO "Open Error, not enoguh memory.\n");
		return -ENOMEM;
	}

	
	printk(KERN_INFO "ZERO: driver open!\n");
	/* check if minor number 1 */
	if (MINOR(geraetedatei->i_rdev) == 1) {
		ptr->available_bytes = strlen(HELLO);
		printk(KERN_INFO "ZERO: ...with minor number 1!\n");
	} else {
		ptr->available_bytes = strlen(ZERO);
		printk(KERN_INFO "ZERO: ...with minor number0!\n");
	}
	
	instanz->private_data = (void*)ptr;
	return EXIT_SUCCESS;
}

static int driver_release(struct inode *geraetedatei, struct file *instanz)
{
	if (instanz->private_data) {
		kfree(instanz->private_data);
	}
	printk(KERN_INFO "ZERO: release driver!\n");
	return EXIT_SUCCESS;
}

static ssize_t driver_read(struct file *instanz, char *user, size_t count, loff_t *offset)
{
	struct myData *ptr;
	
	size_t not_copied, to_copy;
	char *message;
	int minor;

	 
	ptr = (struct myData*)instanz->private_data;

	minor = iminor(instanz->f_path.dentry->d_inode);
	if (minor == 0) {
		message = ZERO;
		printk(KERN_INFO "ZERO: read 0\n");
	} else {
		message = HELLO;
		printk(KERN_INFO "ZERO: read Hello World\n");
	}
	
	to_copy = ptr->available_bytes;
	to_copy = min(to_copy, count);
	if (to_copy <= 0) {
		printk(KERN_INFO "EOF\n");
		/* EOF */
		return 0;
	}
	printk(KERN_INFO "READ %d bytes\n", to_copy);
	
	not_copied = copy_to_user(user, message, to_copy);
	ptr->available_bytes = ptr->available_bytes - to_copy + not_copied;

	return to_copy - not_copied;
}

static int __init ModInit(void)
{
	/* reserve device driver number */
	if(alloc_chrdev_region(&dev_number, 0, NUMBER_OF_MINOR, DRIVER_NAME) < 0)
	{
		printk("ZERO: failed to alloc_chrdev_region\n");	
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
	
	if(cdev_add(driver_object, dev_number, NUMBER_OF_MINOR))
	{
		goto free_cdev;
	}
	
	template_class = class_create(THIS_MODULE, DRIVER_NAME);
	device_create(template_class, NULL, dev_number, NULL, "%s", DRIVER_NAME);
	
	major = MAJOR(dev_number);
	
	printk("Major number: %d\n", major);
	return EXIT_SUCCESS;
	
free_cdev:
	kobject_put(&driver_object->kobj);
	
free_device_number:
	unregister_chrdev_region(dev_number, NUMBER_OF_MINOR);
	return -EIO;
	
}

static void __exit ModExit(void) 
{
	device_destroy(template_class, dev_number);
	class_destroy(template_class);
	cdev_del(driver_object);
	unregister_chrdev_region(dev_number, NUMBER_OF_MINOR);
	return;
}

module_init(ModInit);
module_exit(ModExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Timotheus Ruprecht and  Steffen Gorenflo");
MODULE_DESCRIPTION("Modul MyZero");
MODULE_SUPPORTED_DEVICE("none");


