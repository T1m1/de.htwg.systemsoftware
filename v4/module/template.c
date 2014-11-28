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
	/* on success register chdev returns 0 */
	if((major=register_chrdev(0, "TestDriver", &fobs)) == 0) 
	{
		printk("register_chrdev assigned major %d\n", major);
		return major;
	}
	printk("failed to register_chrdev with %d\n", major);	
	return 0;
}

static void __exit ModExit(void) 
{
	printk("unregister_chred ...");
	unregister_chrdev(major, "TestDriver");
}

module_init(ModInit);
module_exit(ModExit);


