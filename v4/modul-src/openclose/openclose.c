#include <linux/init.h>
#include <linux/module.h>
/* prototypes */
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>


#define DRIVER_NAME "openclose"

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

/* stomic variable */
/* information: makelinux.net/ldd3/chp-5-sect-7 */
static atomic_t lock;


static int driver_open(struct inode *geraetedatei, struct file *instanz)
{
	printk(KERN_INFO "Open driver\n");
	/* check if minor number 1 */
	if (MINOR(geraetedatei->i_rdev) == 1) {
		printk(KERN_INFO "...with minor number 1!\n");
		fobs.read = driver_read_single;
		fobs.write = driver_write_single;
	}
	return EXIT_SUCCESS;
}

static int driver_release(struct inode *geraetedatei, struct file *instanz)
{
	printk(KERN_INFO "Release driver!\n");
	return EXIT_SUCCESS;
}

static ssize_t driver_read(struct file *instanz, char *user, size_t count, loff_t *offset)
{
	return 0;
}

static ssize_t driver_write(struct file *instanz, const char *user, size_t count, loff_t *offset)
{
	return 0;
}

static ssize_t driver_read_single(struct file *instanz, char *user, size_t count, loff_t *offset)
{
	return 0;
}

static ssize_t driver_write_single(struct file *instanz, const char *user, size_t count, loff_t *offset)
{
	return 0;
}


static int __init ModInit(void)
{
	/* reserve device driver number */
	if(alloc_chrdev_region(&dev_number, 0, 1, DRIVER_NAME) < 0){
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
	
	if(cdev_add(driver_object, dev_number, 1)){
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
	unregister_chrdev_region(dev_number, 1);
	return -EIO;
	
}

static void __exit ModExit(void) 
{
	device_destroy(openclose_class, dev_number);
	class_destroy(openclose_class);
	cdev_del(driver_object);
	unregister_chrdev_region(dev_number, 1);
	return;
}

module_init(ModInit);
module_exit(ModExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Timotheus Ruprecht and  Steffen Gorenflo");
MODULE_DESCRIPTION("Modul openclose");
MODULE_SUPPORTED_DEVICE("none");


