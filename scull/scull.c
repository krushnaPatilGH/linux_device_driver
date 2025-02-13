#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>
#include<linux/types.h>
#include<linux/mutex.h>

#define SCULL_MAJOR 0
#define SCULL_MINOR 0
#define SCULL_NR_DEVS 1
#define SCULL_SIZE 1024

static int scull_open(struct inode *inode, struct file *filep);
static int scull_release(struct inode *inode, struct file *filep);
static ssize_t scull_read(struct file *filep, char __user *buf, size_t count, loff_t *offset);
static ssize_t scull_write(struct file *filep, const char __user *buf, size_t count, loff_t *offset);

static struct file_operations scull_fops = {
	.owner = THIS_MODULE,
	.open = scull_open,
	.release = scull_release,
	.read = scull_read,
	.write = scull_write,
};

static int scull_major = SCULL_MAJOR;
static int scull_minor = SCULL_MINOR;
static struct cdev scull_cdev;
static char *scull_memory;
static struct mutex scull_mutex;

static int __init scull_init(void)
{
	int result;
	dev_t dev;

	if (scull_major){
		dev = MKDEV(scull_major, scull_minor);
		result = register_chrdev_region(dev , SCULL_NR_DEVS, "scull");
	}
	else{
		result = alloc_chrdev_region(&dev, scull_minor, SCULL_NR_DEVS, "scull");
		scull_major = MAJOR(dev);
	}	
	if (result < 0){
		printk(KERN_WARNING "scull: can't get a major number\n");
		return result;
	}

	cdev_init(&scull_cdev, &scull_fops);
	scull_cdev.owner = THIS_MODULE;

	result = cdev_add(&scull_cdev, dev, SCULL_NR_DEVS);
	if (result){
		printk(KERN_NOTICE "ERROR %d adding scull\n", result);
		unregister_chrdev_region(dev, SCULL_NR_DEVS);
		return result;
	}

	scull_memory = kmalloc(SCULL_SIZE, GFP_KERNEL);
	if(!scull_memory){
		unregister_chrdev_region(dev, SCULL_NR_DEVS);
		return -ENOMEM;
	}
	printk(KERN_INFO "scull: registered with major number %d\n",scull_major);
	mutex_init(&scull_mutex);
	return 0;
}


static void __exit scull_exit(void)
{
	dev_t dev = MKDEV(scull_major, scull_minor);
	kfree(scull_memory);

	cdev_del(&scull_cdev);

	unregister_chrdev_region(dev , SCULL_NR_DEVS);

	printk(KERN_INFO "scull: device removed\n");
}


static int scull_open(struct inode *inode, struct file *filep)
{
	if (!scull_memory){
		printk(KERN_ERR "scull: memory not allocated\n");
		return -ENOMEM;
	}
	filep->private_data = scull_memory;
	printk(KERN_INFO "scull: device opened\n");

	return 0;
}


static int scull_release(struct inode *inode, struct file *filep)
{
	printk(KERN_INFO "scull: device closed\n");
	return 0;
}


static ssize_t scull_read(struct file *filep, char __user *buf, size_t count, loff_t *offset)
{
	size_t bytes_to_read;

	if (mutex_lock_interruptible(&scull_mutex))
		return -ERESTARTSYS;

	bytes_to_read = min(count, (size_t)(SCULL_SIZE - *offset));
	if (bytes_to_read == 0)
		goto out;

	if (copy_to_user(buf, scull_memory + *offset, bytes_to_read))
	{
		mutex_unlock(&scull_mutex);
		return -EFAULT;
	}

	printk(KERN_INFO "scull_read: reading from device");
	*offset += bytes_to_read;

out:
	mutex_unlock(&scull_mutex);
	return bytes_to_read;
}

static ssize_t scull_write(struct file *filep, const char __user *buf, size_t count, loff_t *offset)
{
	size_t bytes_to_write;


	if(mutex_lock_interruptible(&scull_mutex))
		return -ERESTARTSYS;

	printk(KERN_INFO "scull_write: writing to device \n");
	bytes_to_write = min(count, (size_t)(SCULL_SIZE - *offset));

	if(bytes_to_write == 0)
		goto out;

	if (copy_from_user(scull_memory + *offset, buf, bytes_to_write)){
		mutex_unlock(&scull_mutex);
		return -EFAULT;
	}

	*offset += bytes_to_write;

out:
	mutex_unlock(&scull_mutex);
	return bytes_to_write;
}


module_init(scull_init);
module_exit(scull_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ME");
MODULE_DESCRIPTION("A SIMPLE MOD");
