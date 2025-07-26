#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h> 
#include <linux/uaccess.h>

#define LED_MAJOR   200
#define LED_NAME    "led"

static int led_open(struct inode *inode, struct file *filp)
{
    return 0;
}

static int led_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t led_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
    return 0;
}

/*字符設備操作集*/
static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_release,
    //.read = led_read, // led不需要讀取
    .write = led_write,
};

/*入口*/
static int __init led_init(void)
{
    //註冊一定在入口處
    /*註冊字符設備*/
    int ret = 0;
    ret = register_chrdev(LED_MAJOR, LED_NAME, &led_fops);
    if(ret < 0){
        printk("register chardev failed!\r\n");
        return -EIO;
    }
    printk("led_init\r\n");
    return 0;
}
/*出口*/
static void __exit led_exit(void)
{
    //同理註銷就在出口
    /*註銷字符設備*/
    unregister_chrdev(LED_MAJOR, LED_NAME);
    printk("led_exit\r\n");
}
/*註冊驅動加載和卸載*/
module_init(led_init);
module_exit(led_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("a0000");
MODULE_DESCRIPTION("Simple led register Driver");