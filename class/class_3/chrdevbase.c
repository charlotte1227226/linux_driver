#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init chrdev_init(void)
{
    printk("chrdevbase_init\r\n");

    /*註冊字符設備*/
    int register_chrdev(unsigned int major, const char *name, const struct file_operations *fops);
    return 0;
}
static void __exit chrdev_exit(void)
{
    printk("chrdevbese_exit\r\n");

    /*註銷字符設備*/
    unregister_chrdev(unsigned int major, const char *name);
}

/*
模塊入口與出口
*/

module_init(chrdev_init);  //入口(加載模塊)
module_exit(chrdev_exit);  //出口(卸載模塊)

// 一定要加這行，不然 modpost 會報 missing MODULE_LICENSE()
MODULE_LICENSE("GPL");
// 以下兩行雖非必要，但建議都加上
MODULE_AUTHOR("a0000");
MODULE_DESCRIPTION("Simple Char Device Driver");