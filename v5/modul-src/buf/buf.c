#include <linux/init.h>
#include <linux/module.h>
/* prototypes */
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <asm/atomic.h>
#include <linux/wait.h>
#include <asm/signal.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

#define DRIVER_NAME "buf"

/* normally in stdlib.h */
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define BUF_SIZE 32

typedef struct {
	char * data;
	int index;
	int size;
	int byteCount;
} buffer;


static int buffer_size = BUF_SIZE;
static int major;
static dev_t dev_number;
static struct cdev *driver_object;
struct class *buf_class;
static wait_queue_head_t wait_queue_for_read;
static wait_queue_head_t wait_queue_for_write;
static buffer driver_buffer;

/* prototype functions */
static int driver_open(struct inode *geraetedatei, struct file *instanz);
static int driver_close(struct inode *geraetedatei, struct file *instanz);
static ssize_t driver_read(struct file *instanz, char __user *userbuffer, size_t count, loff_t *offset);
static ssize_t driver_write(struct file *instanz, const char __user *userbuffer, size_t count, loff_t *offs);
int buf_init(buffer *buf, const int size);
int buf_read(buffer *buf, char *out, int byte);
int buf_write(buffer *buf, char *in, int byte);
int buf_destroy(buffer *buf);
int buf_isempty(buffer *buf);
int buf_isfull(buffer *buf);

static struct file_operations fobs =
{
	.owner = THIS_MODULE,
	.open = driver_open,
	.read = driver_read,
	.write = driver_write,
	.release = driver_close,
};

atomic_t bytes_available;
atomic_t bytes_that_can_be_written;

/* define makro for atomic read and write*/
#define READ_POSSIBLE (atomic_read(&bytes_available) != 0)
#define WRITE_POSSIBLE (atomic_read(&bytes_that_can_be_written) != 0)



static int driver_open(struct inode *geraetedatei, struct file *instanz) {
	printk("Device openend\n");	
	return 0;
}

static int driver_close(struct inode *geraetedatei, struct file *instanz) {
	printk("Device released\n");	
	return 0;
}

static ssize_t driver_read(struct file *instanz, char __user *userbuffer, size_t count, loff_t *offset) {
	
	int not_copied, to_copy, retval, copied;
	char tmp[count];

	/* non blocking mode and no data available */
	if (!READ_POSSIBLE && (instanz->f_flags & O_NONBLOCK)) {
		return -EAGAIN;
	}
	
	/* blocking mode:  check available data and maybe wait */
	retval = wait_event_interruptible(wait_queue_for_read, READ_POSSIBLE);	
	
	if (retval == -ERESTARTSYS) {
		printk("Failure reading data: Interrupt by signal\n");
		return -ERESTARTSYS;
	}
	if (retval != 0) {
		printk("Failure reading data\n");
		return -EIO;
	}
		
	to_copy = buf_read(&driver_buffer, tmp, count);
	not_copied = copy_to_user(userbuffer, tmp, to_copy);
	copied = to_copy - not_copied;
	
	atomic_sub(copied, &bytes_available);
	atomic_add(copied, &bytes_that_can_be_written);
	
	/* wake up the writers */
	wake_up_interruptible(&wait_queue_for_write);
	     
	/* return to user application */
    return copied;
			
}

static ssize_t driver_write(struct file *instanz, const char __user *userbuffer, size_t count, loff_t *offs) {
	
	int not_copied, to_copy, retval, copied;
	char tmp[count];
	
	/* non blocking mode and no space to write */
	if (!WRITE_POSSIBLE && (instanz->f_flags & O_NONBLOCK)) {
		return -EAGAIN;
	}
	
	/* blocking mode: check space to write data and maybe wait */
	retval = wait_event_interruptible(wait_queue_for_write, WRITE_POSSIBLE);	
	if (retval == -ERESTARTSYS) {
		printk("Failure writing data: Interrupt by signal\n");
		return -ERESTARTSYS;
	}
	if (retval != 0) {
		printk("Failure writing data\n");
		return -EIO;
	}
	
	to_copy = min((size_t) atomic_read(&bytes_that_can_be_written), count);
	not_copied = copy_from_user(tmp, userbuffer, to_copy);
	copied = to_copy - not_copied;

	buf_write(&driver_buffer, tmp, copied);
		
	atomic_sub(copied, &bytes_that_can_be_written);
	atomic_add(copied, &bytes_available);
	
	/* wake up the readers */
	wake_up_interruptible(&wait_queue_for_read);
	     
	/* return to user application */
    return copied;

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
	
	buf_class = class_create(THIS_MODULE, DRIVER_NAME);
	device_create(buf_class, NULL, dev_number, NULL, "%s", DRIVER_NAME);

	major = MAJOR(dev_number);
	printk("Major number: %d\n", major);
	
	if (!buf_init(&driver_buffer, BUF_SIZE)) {
		return -ENOMEM; 
	}
	atomic_set(&bytes_available, 0);
	atomic_set(&bytes_that_can_be_written, buffer_size);
	
	/* init queues */
	init_waitqueue_head( &wait_queue_for_read );
    init_waitqueue_head( &wait_queue_for_write );
	
	return EXIT_SUCCESS;
	
free_cdev:
	kobject_put(&driver_object->kobj);
	
free_device_number:
	unregister_chrdev_region(dev_number, 1);
	return -EIO;
	
}

static void __exit ModExit(void) 
{
	device_destroy(buf_class, dev_number);
	class_destroy(buf_class);
	cdev_del(driver_object);
	unregister_chrdev_region(dev_number, 1);
	return;
}

int buf_init(buffer *buf, const int size) {
	buf->data = kmalloc(size, GFP_KERNEL);
	if (buf->data == NULL) {
		return 0;
	}
	buf->size = size;
	buf->index = 0;
	buf->byteCount = 0;
	return 1;
}

int buf_read(buffer *buf, char *out, int byte) {
	int i;
	int toRead = min(byte, buf->byteCount);
	for (i = 0; i < toRead; ++i) {
		out[i] = buf->data[buf->index];
		buf->index = (buf->index + 1) % buf->size;
	}
	buf->byteCount -= toRead;
	return toRead;
}

int buf_write(buffer *buf, char *in, int byte) {
	int i, index_w;
	int toWrite = min(byte, buf->size - buf->byteCount);
	for (i = 0; i < toWrite; ++i) {
		index_w = (buf->index + buf->byteCount) % buf->size;
		buf->data[index_w] = in[i];
		++buf->byteCount;
	}
	return toWrite;
}

int buf_destroy(buffer *buf) {
	if (buf->data == NULL){
		return 0;
	}
	
	kfree(buf->data);
	buf->size = -1;
	buf->index = -1;
	buf->byteCount = -1;
	return 1;
}

int buf_isempty(buffer *buf) {
	return buf->byteCount <= 0;
}

	int buf_isfull(buffer *buf) {
	return buf->byteCount >= buf->size;
}

module_init(ModInit);
module_exit(ModExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Timotheus Ruprecht and  Steffen Gorenflo");
MODULE_DESCRIPTION("Modul Template");
MODULE_SUPPORTED_DEVICE("none");


