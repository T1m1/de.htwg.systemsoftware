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
/* register for gpio pin 10 - 19 */
#define GPFSEL1 0xF2200004
/* register for gpio pin 20 - 29 */
#define GPFSEL2 0xF2200008
/* register for gpio values 0 - 31 */
#define GPLEV0 0xF2200034

#define GPFSEL(pin) (u32*)(gpio + (pin / 10))
#define GPFSET(pin) (u32*)(gpio + 7 + (pin / 32))
#define GPFCLR(pin) (u32*)(gpio + 10 + (pin / 32))
#define GPFLEV(pin) (u32*)(gpio + 13 + (pin / 32))
#define GPIOLEV gpio + 13

/* address for gpio 18 and 25 */
#define GPIO_18 (gpio + (18 / 10))
#define GPIO_25 (gpio + (25 / 10))

#define GPIO_OUT 18
#define GPIO_IN 25

/* mask to clear bits */
#define CLEAR_GPIO_18 0xF8FFFFFF
#define CLEAR_GPIO_25 0xFFFC7FFF

/* direction of gpio pins */
#define GPIO_18_AS_OUTPUT 0x01000000
#define GPIO_25_AS_INPUT 0x000C7000

/* HIGH - LOW for GPIO 18 */
#define GPIO_HIGH_18 0x00040000
#define GPIO_LOW_18 0x00040000

/* MASK to check GPIO 25 */
#define GPIO_25_MASK 0x00008000

/* VALUE of GPIO 25 */
#define GPIO_25_VALUE GPFLEV(25)

/*************************************************************/
/*PROBLEM - virtual address translate - example from google */
#define BCM2708_PERI_BASE 0x20000000
#define GPIO_BASE (BCM2708_PERI_BASE + 0x200000)
#define MEM_REG_LEN 4096

static volatile unsigned *gpio;
/*************************************************************/

static int major;
static struct file_operations fobs;
static dev_t dev_number;
static struct cdev *driver_object;
struct class *mygpio_class;

static int driver_open(struct inode *geraetedatei, struct file *instanz);
static void gpio_init(int gpio_pin, unsigned long gpio_clear, unsigned long gpio_direction);
static ssize_t driver_write(struct file *instanz, const char *user, size_t count, loff_t *offset);
static void gpio_write(unsigned long gpio_addr,  int gpio_value);
static ssize_t driver_read(struct file *instanz, char *user, size_t count, loff_t *offset);


static int driver_release(struct inode *geraetedatei, struct file *instanz)
{
	return 0;
}

static struct file_operations fobs =
{
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_release,
	.write = driver_write,
	.read = driver_read
};

static ssize_t driver_read(struct file *instanz, char *user, size_t count, loff_t *offset)
{
	size_t to_copy, not_copied;
	char value;
	u32 *ptr;
	u32 old_value, bitmask;
	
	/* check size of parameter is one byte */
	if (1 != count) {
		printk(KERN_INFO "read: can only write 1 byte!\n");
		return -EAGAIN;
	}
	
	/* read value from GPIO_25 */
	ptr = (u32 *)GPFLEV(GPIO_IN);
	old_value = readl(ptr);
	
	bitmask = 0x1 << GPIO_IN;
	old_value = old_value & bitmask;
	
	/* after read - add memory barrier */
	rmb();
	
	/* check register */
	if(old_value != bitmask ) {
		value = '1';
	} else {
		value = '0';
	}
	printk(KERN_INFO "gpio-read value: %c\n", value);
	/* read value */
	to_copy = ONE_BYTE;
	to_copy = min(to_copy, count);
	not_copied = copy_to_user(user, &value, to_copy);

	return to_copy - not_copied;


}

static ssize_t driver_write(struct file *instanz, const char *user, size_t count, loff_t *offset)
{
	size_t not_copied;
	char value[count];
	u32 *ptr;
	
	/* copy byte form user */
	not_copied = copy_from_user(&value, user, count);
	
	/*printk(KERN_DEBUG "write: %s\n", user);*/
	/* check for correct value of element */
	if('1' != value[0] && '0' != value[0]) {
		printk(KERN_INFO "write: Wrong value! Can only write 1 or 0!\n");
		printk(KERN_INFO "write: size = %d!\n", count);
		return -EAGAIN;
	}
	
	if('0' == value[0]) {
		/** LED ON **/
		/* set bit of GPIO_18 to high */
		gpio_write(GPIO_HIGH_18, 1);
	} 
	if('1' == value[0]) {
		/** LED OFF **/
		/* set bit of GPIO_18 to low */	
		gpio_write(GPIO_LOW_18, 0);
	}

	/*printk(KERN_INFO "gpio-write value: %c\n", value[0]);*/
	
	return count;
}

static void gpio_write(unsigned long gpio_addr, int gpio_value ) 
{
	u32 *ptr;
	if(1 == gpio_value) {
		/** LED OFF **/
		ptr = (u32 *)GPFCLR(GPIO_OUT);
	} else {
		/** LED ON **/
		ptr = (u32 *)GPFSET(GPIO_OUT);
	}
	/* after read and before write- add memory barrier */
	mb();
	/* write new value to gpio */
	writel(gpio_addr, ptr);
}

static int driver_open(struct inode *geraetedatei, struct file *instanz)
{
	printk(KERN_INFO "open gpio 18\n");
	/* init gpio 18 as output */
	gpio_init(GPIO_OUT, CLEAR_GPIO_18, GPIO_18_AS_OUTPUT);
	
	printk(KERN_INFO "open gpio 25\n");
	/* init gpio 25 as input */
	gpio_init(GPIO_IN, CLEAR_GPIO_25, GPIO_25_AS_INPUT);
	
	return EXIT_SUCCESS;
}

static void gpio_init(int gpio_pin, unsigned long gpio_clear, unsigned long gpio_direction) 
{
	u32 *ptr = GPFLEV(gpio_pin);
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
	//writel(old_value | gpio_direction, ptr);
	*ptr = old_value | gpio_direction;
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
	
	/* virtual to phsical address */
	gpio = ioremap(GPIO_BASE, MEM_REG_LEN);
	
	if(cdev_add(driver_object, dev_number, 1))
	{
		goto free_cdev;
	}
	
	mygpio_class = class_create(THIS_MODULE, DRIVER_NAME);
	device_create(mygpio_class, NULL, dev_number, NULL, "%s", DRIVER_NAME);
	
	major = MAJOR(dev_number);
	
	printk("Majore number: %d\n", major);
	return EXIT_SUCCESS;
	
free_cdev:
	kobject_put(&driver_object->kobj);
	
free_device_number:
	unregister_chrdev_region(dev_number, 1);
	return -EIO;
}

static void __exit ModExit(void) 
{
	release_mem_region(GPIO_BASE, MEM_REG_LEN);
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


