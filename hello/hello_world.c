#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>

static char *whom = "world";
static int howmany = 1;
module_param(howmany, int, S_IRUGO);
module_param(whom, charp, S_IRUGO);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("ZYRO");
MODULE_DESCRIPTION("HELLO MODULE");

static int hello_world(void)
{
	printk(KERN_ALERT "HELLO WORLD\n");
	return 0;
}

static void exit_world(void)
{
	printk(KERN_ALERT "EXIT WORLD\n");
}

module_init(hello_world);
module_exit(exit_world);
