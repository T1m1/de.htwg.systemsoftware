#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Timotheus Ruprecht and  Steffen Gorenflo");
MODULE_DESCRIPTION("Modul Template");
MODULE_SUPPORTED_DEVICE("none");


static int __init ModInit(void)
{
	printk(KERN_ALERT "HELLO, world\n");
	return 0;
}

static void __exit ModExit(void)
{
	printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(ModInit);
module_exit(ModExit);


