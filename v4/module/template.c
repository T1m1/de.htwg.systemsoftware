#include <linux/init.h>
#include <linux/module.h>
/* prototypes */
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Timotheus Ruprecht and  Steffen Gorenflo");
MODULE_DESCRIPTION("Modul Template");
MODULE_SUPPORTED_DEVICE("none");

static int major;
static struct file_operations fobs;

static int __init ModInit(void)
{
	printk(KERN_ALERT "HELLO, world\n");
	/* on success register chdev returns 0 */
	if((major=register_chrdev(0, "TestDriver", &fobs)) == 0) 
	{
		return major;
	}	
	return 0;
}

static void __exit ModExit(void) 
{
	printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(ModInit);
module_exit(ModExit);


