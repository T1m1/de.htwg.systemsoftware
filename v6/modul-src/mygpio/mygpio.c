#include <linux/init.h>
#include <linux/module.h>
/* prototypes */
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DRIVER_NAME "mygpio"

/* normaly in stdlib.h */
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

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


static int major;
static struct file_operations fobs;
static dev_t dev_number;
static struct cdev *driver_object;
struct class *mygpio_class;

static int driver_open(struct inode *geraetedatei, struct file *instanz);

static struct file_operations fobs =
{
	.owner = THIS_MODULE,
	.open = driver_open,
};

static int driver_open(struct inode *geraetedatei, struct file *instanz)
{
	u32 *ptr_gpio18 = (u32 *)GPIO_18;
	u32 *ptr_gpio25 = (u32 *)GPIO_25;
	
	u32 old_value;
	
	printk(KERN_INFO "gpio pin 18 as output");
	old_value = readl(ptr_gpio18);
	/* clear bits for GPIO-18 */
	old_value = old_value & CLEAR_GPIO_18;
	/* configure GPIO-18 as output */
	writel(old_value | GPIO_18, ptr_gpio18);
	
	printk(KERN_INFO "gpio pin 25 as input");
	old_value = readl(ptr_gpio25);
	/* clear bits for GPIO-25 */
	old_value = old_value & CLEAR_GPIO_25;
	/* configure GPIO-25 as output */
	writel(old_value | GPIO_25, ptr_gpio25);
	
	return EXIT_SUCCESS;
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


