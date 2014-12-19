#include <linux/init.h>
#include <linux/module.h>
/* prototypes */
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include <linux/ioctl.h>

/* kthread */
#include <linux/kthread.h>

#define DRIVER_NAME "kthread"

/* normaly in stdlib.h */
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

static int major;
static struct file_operations fobs;
static dev_t dev_number;
static struct cdev *driver_object;
struct class *ktrhead_class;

/* thread specific */
static struct task_struct* thread_id;
static wait_queue_head_t wq;
static DECLARE_COMPLETION(on_exit);

static int thread_code(void *data)
{
	unsigned long timeout;

	allow_signal( SIGTERM );
	init_waitqueue_head( &wq );
	printk("thread_code startet ...\n");
	while( !signal_pending(current) ) {
		/* wait 2 second */
		timeout= 2 * HZ;
		timeout=wait_event_interruptible_timeout(
			wq, (timeout==0),timeout );
		printk("thread_function: wake up... after 2 seconds!\n");
		if( timeout==-ERESTARTSYS ) {
			printk("got signal...\n");
			break;
		}
	}
	complete_and_exit( &on_exit, 0 );
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
	
	ktrhead_class = class_create(THIS_MODULE, DRIVER_NAME);
	device_create(ktrhead_class, NULL, dev_number, NULL, "%s", DRIVER_NAME);
	
	
	/* thread init */
	init_waitqueue_head(&wq);
	thread_id=kthread_create(thread_code, NULL, "mykthread");
	if(thread_id == 0) {
		return -EIO;
	}
	wake_up_process(thread_id);
		
	
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
	/* kill thread */
	kill_pid(task_pid(thread_id), SIGTERM, 1);
	wait_for_completion(&on_exit);
	
	device_destroy(ktrhead_class, dev_number);
	class_destroy(ktrhead_class);
	cdev_del(driver_object);
	unregister_chrdev_region(dev_number, 1);
	return;
}

module_init(ModInit);
module_exit(ModExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Timotheus Ruprecht and  Steffen Gorenflo");
MODULE_DESCRIPTION("Modul Ktrhead");
MODULE_SUPPORTED_DEVICE("none");


