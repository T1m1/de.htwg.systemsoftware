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

#define DRIVER_NAME "timer"

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
/* time cycles */
cycles_t min_cyc, max_cyc, cur_cyc, last_cyc = 0;
	

static void inc_count(unsigned long arg)
{	
	cur = jiffies -last;
	cur_cyc = get_cycles() - last_cyc;
	
	//number_of_cycles get_cycles(void);
	if( last_cyc ) {
		if( cur_cyc > max_cyc ) {
			max_cyc = cur_cyc;
		}
		if( cur_cyc < min_cyc ) {
			min_cyc = cur_cyc;
		}
	}
	last_cyc = get_cycles();
	
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
	printk("inc_count called (%ld)... \ncurrent (cycles): %d\nmin (cycles): %d\nmax (cycles): %d\n",
			mytimer.expires, (int) cur_cyc, (int) min_cyc, (int) max_cyc );
			
    /* call timer again in 2 sec */
    mytimer.expires = jiffies + (2*HZ); 
    /* check if timer should be added again */
    if (atomic_read(&stop_timer)) {
		complete(&on_exit);
		printk(KERN_INFO "inc_count: do not call add_timer");
	} else {
		add_timer(&mytimer);
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
	
	init_timer( &mytimer );
    mytimer.function = inc_count;
    mytimer.data = 0;
    mytimer.expires = jiffies + (2*HZ); // 2 second
    add_timer( &mytimer );
    printk("Timer init done!\n");
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
	printk(KERN_INFO "ModExit after timer finished\n");
        
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


