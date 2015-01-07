#include <linux/init.h>
#include <linux/module.h>
/* prototypes */
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
/* mutex */
#include <linux/mutex.h>
#include <linux/sched.h>

#define DRIVER_NAME "open_once"

/* normaly in stdlib.h */
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

static int major;
static dev_t dev_number;
static struct cdev *driver_object;
struct class *mutex_class;

/* define mutex */
struct mutex my_mutex;
DEFINE_MUTEX(my_mutex);

static int driver_open( struct inode *geraetedatei, struct file *instanz );
static ssize_t driver_read(struct file *instanz, char *user, size_t count, loff_t *offset);


static struct file_operations fobs =
{
	.owner = THIS_MODULE,
	.open = driver_open,
	.read = driver_read,
};

static int driver_open( struct inode *geraetedatei, struct file *instanz )
{
	/* try to get the mutex */
	while( mutex_trylock( &my_mutex ) == 0 ) {
		printk("MUTEX: busy (%ld)...\n", jiffies);
		/* wait 200 mili seconds */
		schedule_timeout_interruptible( 200*HZ/1000 );
		if( signal_pending(current) ) {
			mutex_unlock( &my_mutex );
			return -EIO;
		}
	}
	printk("MUTEX: sleeping 3 seconds -> start\n");
	/* sleep for 3 seconds */
	schedule_timeout_interruptible( 3*HZ );
	printk("MUTEX: sleeping 3 seconds -> finish\n");
	mutex_unlock( &my_mutex );
	return 0;
}

static ssize_t driver_read(struct file *instanz, char *user, size_t count, loff_t *offset)
{
	printk(KERN_INFO "MUTEX: read called...\n");
	return 0;
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
	
	mutex_class = class_create(THIS_MODULE, DRIVER_NAME);
	device_create(mutex_class, NULL, dev_number, NULL, "%s", DRIVER_NAME);
	
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
	device_destroy(mutex_class, dev_number);
	class_destroy(mutex_class);
	cdev_del(driver_object);
	unregister_chrdev_region(dev_number, 1);
	return;
}

module_init(ModInit);
module_exit(ModExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Timotheus Ruprecht and  Steffen Gorenflo");
MODULE_DESCRIPTION("Modul Open Once");
MODULE_SUPPORTED_DEVICE("none");


