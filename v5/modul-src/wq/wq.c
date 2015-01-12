#include <linux/init.h>
#include <linux/module.h>
/* prototypes */
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
/* timer */
#include <linux/timer.h>
#include <linux/sched.h>
/* time measurement */
#include <linux/cpufreq.h>
#include <linux/timex.h>

#include <linux/workqueue.h>
#include <linux/delay.h>

#define DRIVER_NAME "wq"

/* normaly in stdlib.h */
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

static int major;
static struct file_operations fobs;
static dev_t dev_number;
static struct cdev *driver_object;
struct class *template_class;

static struct timer_list mytimer;
static atomic_t stop_timer = ATOMIC_INIT(0);
static DECLARE_COMPLETION(on_exit);

/* timer calculation */
static unsigned int min, max, cur, last = 0;

/* workque */
static struct workqueue_struct *wq;
static void work_queue_func(struct work_struct*);

static DECLARE_WORK(work_obj, work_queue_func);
	

static void work_queue_func(struct work_struct *work)
{	
	cur = jiffies -last;
	
	if( last ) {
		if( cur > max ) {
			max = cur;
		}
		if( cur < min ) {
			min = cur;
		}
	}
	last = jiffies;
	
	
    printk("inc_count called (%ld)... \ncurrent: %d\nmin: %d\nmax: %d\n",
			mytimer.expires, cur, min, max );
			
	msleep(2000);
	
    /* check if timer should be added again */
    if (atomic_read(&stop_timer)) {
		complete(&on_exit);
		return;
	} 
	
	if (queue_work(wq, &work_obj)) {
		printk(KERN_INFO "queue_work SUCCESS\n");
	} else {
		printk(KERN_INFO "queue_work ERROR\n");
	}
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
	
	template_class = class_create(THIS_MODULE, DRIVER_NAME);
	device_create(template_class, NULL, dev_number, NULL, "%s", DRIVER_NAME);
	
	major = MAJOR(dev_number);
	printk("Major number: %d\n", major);
	
	/* wq */
	wq = create_workqueue("my_wq");
	if (queue_work(wq, &work_obj)) {
		printk(KERN_INFO "queue_work SUCCESS\n");
	} else {
		printk(KERN_INFO "queue_work ERROR\n");
		goto free_cdev;
	}
	

	return EXIT_SUCCESS;
	
free_cdev:
	kobject_put(&driver_object->kobj);
	
free_device_number:
	unregister_chrdev_region(dev_number, 1);
	return -EIO;
	
}

static void __exit ModExit(void) 
{
	atomic_set(&stop_timer, 1);
	wait_for_completion(&on_exit);
	
	pr_debug("ModExit called\n");
	/* destroy workqueue */
	if (wq) {
		destroy_workqueue(wq);
		printk(KERN_INFO "workqueue detroyed\n");
	}
        
	device_destroy(template_class, dev_number);
	class_destroy(template_class);
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


