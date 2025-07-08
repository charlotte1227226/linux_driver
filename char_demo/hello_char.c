#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define DEVICE_NAME "hello_char"
static int major;

// open
static int hello_open(struct inode *inode, struct file *filp) {
    pr_info("hello_char: open()\n");
    return 0;
}
// read
static ssize_t hello_read(struct file *filp, char __user *buf, size_t len, loff_t *offset) {
    const char *msg = "Hello from PI!\n";
    size_t msg_len = strlen(msg);

    if (*offset >= msg_len) return 0;             // EOF
    if (len > msg_len - *offset) len = msg_len - *offset;

    if (copy_to_user(buf, msg + *offset, len)) return -EFAULT;
    *offset += len;
    return len;
}
// release
static int hello_release(struct inode *inode, struct file *filp) {
    pr_info("hello_char: release()\n");
    return 0;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = hello_open,
    .read    = hello_read,
    .release = hello_release,
};

static int __init hello_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("hello_char: register_chrdev failed\n");
        return major;
    }
    pr_info("hello_char: registered with major %d\n", major);
    return 0;
}

static void __exit hello_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("hello_char: unregistered\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("a0000");
MODULE_DESCRIPTION("Simple Hello Char Driver");
