/*
* this file contains the source code for the lkm_sort.ko module
* it provides a __very__ simple sorting facility for usermode
* processes, wishing to sort buffers of character data.
* it only handles char buffers [simple char type].
*/


#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/uaccess.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eran Ben-Horin");
MODULE_DESCRIPTION("Deep Instinct Sorting Module");
MODULE_VERSION("1.0");

#define DEVICE_NAME "lkm_sort"
#define MAX_MEM_ALLOC 1024 * 1024

/* Prototypes for device functions */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static int major_num;
static int device_open_count = 0;
static char *kernel_buffer = 0;

/* external util functions */
static void quick_sort(char A[], int p, int r);

/* This structure points to all of the device functions */
static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

/* When a process reads from our device, this gets called. */
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) {
    printk(KERN_ALERT "device_read");
    if (copy_to_user(buffer, kernel_buffer, len) != 0){
        printk(KERN_ALERT "device_read: copy to user failed");
        return -EFAULT;
    }
    kfree(kernel_buffer);
    kernel_buffer = 0;
    return len;
}
    
/* Called when a process tries to write to our device */
static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset) {

    printk(KERN_ALERT "device_write");
    if (NULL == buffer || 0 >= len){
        printk(KERN_ALERT "device_write: bad argument\n");
        return -EINVAL;
    }

    if (len > MAX_MEM_ALLOC){
        printk(KERN_ALERT "device_write: len exceeds maximum memory size\n");
        return -EINVAL;
    }

    // we dont allow parallel use of the module
    if (NULL != kernel_buffer){
        return -EBUSY;
    }

    // we copy the buffer sent from usermode, so that we can
    // suport the read-by-chunks feature, and also because we
    // run an in-place sorting algorithm
    kernel_buffer = kmalloc(len, GFP_KERNEL);
    if(!kernel_buffer){
        printk(KERN_ALERT "device_write: memory allocation failed\n");
        return -ENOMEM;
    }
    if (copy_from_user(kernel_buffer, buffer, len) != 0){
        printk(KERN_ALERT "device_write: copy from user failed\n");
        return -EFAULT;
    }
    
    // invoke q-sort
    quick_sort(kernel_buffer, 0, len-1);

    return len;
}
    /* Called when a process opens our device */
static int device_open(struct inode *inode, struct file *file) {
    printk(KERN_ALERT "device_open");
    /* If device is open, return busy */
    if (device_open_count) {
        return -EBUSY;
    }
    device_open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}
    /* Called when a process closes our device */
static int device_release(struct inode *inode, struct file *file) {
    /* Decrement the open counter and usage count. Without this, the module would not unload. */
    device_open_count--;
    module_put(THIS_MODULE);
    return 0;
}

static int __init lkm_sort_init(void) {
    printk(KERN_INFO "loading lkm_sort for deep-instinct");
    major_num = register_chrdev(0, DEVICE_NAME, &file_ops);
    if (major_num < 0) {
        printk(KERN_ALERT "Could not register device: %d\n", major_num);
        return major_num;
    }else {
        printk(KERN_INFO "lkm_sort module loaded with device major number %d\n", major_num);
    return 0;
    }
}


static void __exit lkm_sort_exit(void) {
    /* Remember — we have to clean up after ourselves. Unregister the character device. */
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_INFO "unloading lkm_sort for deep-instinct");
}


/**
 * a __super__ naiive implementation of quick sort
 */
#define naiive_swap(t, x, y) { t z = x; x = y; y = z; }

int partition(char A[], int p, int r) {
	char x = A[r]; //pivot
	int i = p - 1, j;
	for(j = p; j < r ; j++) {
		if(A[j] <= x) {
			i = i + 1;
			naiive_swap(char, A[i], A[j]); 
		}
	}
	i = i + 1;
	naiive_swap(char, A[i], A[r]);
	return i;
}

void quick_sort(char A[], int p, int r) {
    int rand_num = 0;
    int t = 0, q = 0;
	if(p < r) {
        get_random_bytes(&rand_num, sizeof(rand_num)-1);
		t = (rand_num % ( r - p + 1) + p);
		naiive_swap(char, A[t], A[r]); 
		//Used to avoid O(n^2) worst case
		
		q = partition(A, p, r);
		quick_sort(A, p, q - 1);
		quick_sort(A, q + 1, r);
	}
}



/* Register module functions */
module_init(lkm_sort_init);
module_exit(lkm_sort_exit);