#include <linux/kernel.h>
#include <linux/linkage.h>

asmlinkage long sys_csci3753_add(int x, int y, int* z) {
	printk(KERN_ALERT "%1d and %2d will be added\n", x, y);
	*z = x + y;
	printk(KERN_ALERT "Result is %d\n", *z);
	return 0;
}
