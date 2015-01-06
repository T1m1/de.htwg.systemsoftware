#include <linux/init.h>
#include <linux/module.h>
/* prototypes */
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
/* header for io control */
#include <asm/io.h>
/* header for copy to/from user */
#include <linux/uaccess.h>

#define DRIVER_NAME "mygpio"

/* normaly in stdlib.h */
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/* 1 BYTE */
#define ONE_BYTE 1

/** register for GPIO **/
/* gpio pin 10 - 19 */
#define GPFSEL1 0xF2200004
/* gpio pin 20 - 29 */
#define GPFSEL2 0xF2200008

/* address for gpio 18 and 25 */
#define GPIO_18 GPFSEL1
#define GPIO_25 GPFSEL2

/* mask to clear bits */
#define CLEAR_GPIO_18 0xF8FFFFFF
#define CLEAR_GPIO_25 0xFFFC7FFF

#define GPIO_18_AS_OUTPUT 0x01000000
#define GPIO_25_AS_INPUT 0x000C7000


static int major;
static struct file_operations fobs;
static dev_t dev_number;
static struct cdev *driver_object;
struct class *mygpio_class;

static int driver_open(struct inode *geraetedatei, struct file *instanz);
static void gpio_init(unsigned long gpio_address, unsigned long gpio_clear, unsigned long gpio_direction);
static ssize_t driver_write(struct file *instanz, const char *user, size_t count, loff_t *offset);

static struct file_operations fobs =
{
	.owner = THIS_MODULE,
	.open = driver_open,
	.write = driver_write
};

static ssize_t driver_write(struct file *instanz, const char *user, size_t count, loff_t *offset)
{
	
	size_t to_copy, not_copied;
	char value;
	
	/* check size of parameter is one byte */
	if (1 != count) {
		printk(KERN_INFO "write: can only write 1 byte!\n");
		return -EAGAIN;
	}
	
	/* copy byte form user */
	to_copy = ONE_BYTE;
	to_copy = min(to_copy, count);
	not_copied = copy_from_user(&value, user, to_copy);
	
	/* check for correct value of element */
	if('1' != value && '0' != value) {
		printk(KERN_INFO "write: Wrong value! Can only write 1 or 0!\n");
		return -EAGAIN;
	}
	
	return to_copy - not_copied;
}

static int driver_open(struct inode *geraetedatei, struct file *instanz)
{
	/* init gpio 18 as output */
	gpio_init(GPIO_18, CLEAR_GPIO_18, GPIO_18_AS_OUTPUT);
	/* init gpio 25 as input */
	gpio_init(GPIO_25, CLEAR_GPIO_25, GPIO_25_AS_INPUT);
	
	return EXIT_SUCCESS;
}

static void gpio_init(unsigned long gpio_address, unsigned long gpio_clear, unsigned long gpio_direction) 
{
	u32 *ptr = (u32 *)gpio_address;
	u32 old_value;
	
	/* read value */
	old_value = readl(ptr);
	/* after read - add memory barrier */
	rmb();
	/* clear bits */
	old_value = old_value & gpio_clear;
	/* before write - add memory barrier */
	wmb();
	/* configure direction */
	writel(old_value | gpio_direction, ptr);
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
	
	mygpio_class = class_create(THIS_MODULE, DRIVER_NAME);
	device_create(mygpio_class, NULL, dev_number, NULL, "%s", DRIVER_NAME);
	
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
	device_destroy(mygpio_class, dev_number);
	class_destroy(mygpio_class);
	cdev_del(driver_object);
	unregister_chrdev_region(dev_number, 1);
	return;
}

module_init(ModInit);
module_exit(ModExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Timotheus Ruprecht and  Steffen Gorenflo");
MODULE_DESCRIPTION("Modul MyGpio");
MODULE_SUPPORTED_DEVICE("none");


