#include <linux/module.h>

static int __init chrdev_init(void)
{
    return 0;
}
static void __exit chrdev_exit(void)
{

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