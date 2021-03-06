#include <linux/init.h>
#include <linux/module.h>
/* prototypes */
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/errno.h>

#define DRIVER_NAME "openclose"
#define MAX_NUMBER_OF_PROCESS 1
#define NUMBER_OF_MINOR 2

/* normaly in stdlib.h */
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1


static int major;
static dev_t dev_number;
static struct cdev *driver_object;
struct class *openclose_class;

static int driver_open(struct inode *geraetedatei, struct file *instanz);
static int driver_release(struct inode *geraetedatei, struct file *instanz);

static ssize_t driver_read(struct file *instanz, char *user, size_t count, loff_t *offset);
static ssize_t driver_write(struct file *instanz, const char *user, size_t count, loff_t *offset);

static ssize_t driver_read_single(struct file *instanz, char *user, size_t count, loff_t *offset);
static ssize_t driver_write_single(struct file *instanz, const char *user, size_t count, loff_t *offset);



static struct file_operations fobs =
{
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_release,
	.read = driver_read,
	.write = driver_write
};

/* atomic variable */
/* information: makelinux.net/ldd3/chp-5-sect-7 */
static atomic_t lock = ATOMIC_INIT(MAX_NUMBER_OF_PROCESS);


static int driver_open(struct inode *geraetedatei, struct file *instanz)
{
	printk(KERN_INFO "Try to open driver..\n");
	/* check if minor number 1 */
	if (MINOR(geraetedatei->i_rdev) == 1) {
		printk(KERN_INFO "...with minor 1!\n");
		if (!atomic_dec_and_test(&lock)) {
			atomic_inc(&lock);
			printk(KERN_INFO "Diver already in use!\n");
			return -EBUSY;
		}
		fobs.read = driver_read_single;
		fobs.write = driver_write_single;
		printk(KERN_INFO "Driver open with minor 1!\n");
	} else {
		printk(KERN_INFO "...with minor 0!\n");
		/* if called with minor 0 after called with minor 1 */
		fobs.read = driver_read;
		fobs.write = driver_write;
		printk(KERN_INFO "Driver open with minor 0!\n");
	}
	return EXIT_SUCCESS;
}

static int driver_release(struct inode *geraetedatei, struct file *instanz)
{
	if (MINOR(geraetedatei->i_rdev) == 1) {
		atomic_inc(&lock);
		printk(KERN_INFO "Release driver with minor 1!\n");
	} else {
		printk(KERN_INFO "Release driver with minor 0!\n");
	}
	
	return EXIT_SUCCESS;
}

static ssize_t driver_read(struct file *instanz, char *user, size_t count, loff_t *offset)
{
	printk(KERN_INFO "NORMAL read called...\n");
	return 0;
}

static ssize_t driver_write(struct file *instanz, const char *user, size_t count, loff_t *offset)
{
	printk(KERN_INFO "NORMAL write called...\n");
	return 0;
}

static ssize_t driver_read_single(struct file *instanz, char *user, size_t count, loff_t *offset)
{
	printk(KERN_INFO "SINGLE read called...\n");
	return 0;
}

static ssize_t driver_write_single(struct file *instanz, const char *user, size_t count, loff_t *offset)
{
	printk(KERN_INFO "SINGLE write called...\n");
	return 0;
}


static int __init ModInit(void)
{
	/* reserve device driver number */
	if(alloc_chrdev_region(&dev_number, 0, NUMBER_OF_MINOR, DRIVER_NAME) < 0){
		printk("failed to alloc_chrdev_region\n");	
		return -EIO;
	}
	/* reserve object */
	driver_object = cdev_alloc();
	
	if(driver_object == NULL){
		goto free_device_number;
	}
	driver_object->owner = THIS_MODULE;
	driver_object->ops = &fobs;
	
	if(cdev_add(driver_object, dev_number, NUMBER_OF_MINOR)){
		goto free_cdev;
	}
	
	openclose_class = class_create(THIS_MODULE, DRIVER_NAME);
	device_create(openclose_class, NULL, dev_number, NULL, "%s", DRIVER_NAME);
	
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
	device_destroy(openclose_class, dev_number);
	class_destroy(openclose_class);
	cdev_del(driver_object);
	unregister_chrdev_region(dev_number, NUMBER_OF_MINOR);
	return;
}

module_init(ModInit);
module_exit(ModExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Timotheus Ruprecht and  Steffen Gorenflo");
MODULE_DESCRIPTION("Modul openclose");
MODULE_SUPPORTED_DEVICE("none");