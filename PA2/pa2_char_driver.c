#include<linux/uaccess.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/slab.h>
#include<asm/uaccess.h>


// global defs/vars for buffer, device name, major number
#define BUFFER_SIZE 1024
#define DEVICE_NAME "simple_character_device"

static int majorNumber=122;
static char* deviceBuffer;



ssize_t simple_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset) {

	// limit to reading only to end of buffer
	if (length > BUFFER_SIZE - *offset) length = BUFFER_SIZE - *offset;

	// if copy returns fault, print error to log
	if (copy_to_user(buffer, deviceBuffer + *offset, length)) printk(KERN_ALERT "ERROR: some bytes unread\n");
	else printk(KERN_ALERT "bytes read: %zd\n", length);

	// set offset just after read bytes
	*offset = *offset + length;

	return 0;
}



ssize_t simple_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) {


	// limit writing to end of buffer
	if (length > BUFFER_SIZE - *offset) length = BUFFER_SIZE - *offset;

	// if copy returns fault, print error to log
	if(copy_from_user(deviceBuffer + *offset, buffer, length)) printk(KERN_ALERT "ERROR: some bytes no written\n");
	else printk(KERN_ALERT "Bytes Writen: %zd\n", length);

	// set offset to just after written string
	*offset = *offset + length;
	
	return length;
}


int simple_char_driver_open (struct inode *pinode, struct file *pfile) {

	// log num times opened to print to log
	static int opens = 0;
	opens++;

	printk(KERN_ALERT "Device opened: %d times\n", opens);
	return 0;
}

int simple_char_driver_close (struct inode *pinode, struct file *pfile) {

	// log num times closed and print to log
	static int closes = 0;
	closes++;

	printk(KERN_ALERT "Device closed: %d times\n", closes);
	return 0;
}

loff_t simple_char_driver_seek (struct file *pfile, loff_t offset, int whence) {

	// update position within file based on user input
	loff_t newPosition = 0;
	
	switch(whence) {

	// SEEK_SET
	case 0:
		newPosition = offset;
		break;

	// SEEK_CUR
	case 1:
		newPosition = pfile -> f_pos + offset;
		break;

	// SEEK_END
	case 2:
		newPosition = BUFFER_SIZE + offset;
		break;
	}

	// error checking for before or after bounds of buffer

	if(newPosition > BUFFER_SIZE - 1) {
		newPosition = pfile -> f_pos;
		printk(KERN_ALERT "Error: Attempting to seek beyond end of file\n");
	}
	else if(newPosition < 0) {
		newPosition = pfile -> f_pos;
		printk(KERN_ALERT "Error: Attempting to seek before beginning of file\n");
	}

	// finally update file position
	pfile -> f_pos = newPosition;

	return 0;
}

struct file_operations simple_char_driver_file_operations = {

	// pointers to function definitions

	.owner   = THIS_MODULE,
	.read = simple_char_driver_read,
	.write = simple_char_driver_write,
	.open = simple_char_driver_open,
	.release = simple_char_driver_close,
	.llseek = simple_char_driver_seek,
};

static int simple_char_driver_init(void) {

	// print to log init called, register device, allocate memory for buffer

	deviceBuffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);

	if(register_chrdev(majorNumber, DEVICE_NAME, &simple_char_driver_file_operations) < 0) {
		printk(KERN_ALERT "Registering character device failed with %d\n", majorNumber);
    	return majorNumber;
	}

	printk(KERN_ALERT "inside %s function\n", __FUNCTION__);	

	return 0;
}

static void simple_char_driver_exit(void) {
	
	// print to log exit function called, unregister device, free buffer space

	unregister_chrdev(majorNumber, DEVICE_NAME);
	
	printk(KERN_ALERT "inside %s funciton\n", __FUNCTION__);

	memset(deviceBuffer, '\0', BUFFER_SIZE);
	kfree(deviceBuffer);
}

// define init and exit function accordingly
module_init(simple_char_driver_init);
module_exit(simple_char_driver_exit);
