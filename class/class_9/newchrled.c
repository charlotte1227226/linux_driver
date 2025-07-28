#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h> 
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/types.h>

#define NEWCHRLED_NAME "newchrled"
#define NEWCHRLED_COUNT 1

#define LEDOFF  0   //關閉
#define LEDON   1   //打開

/*LED 設備結構體*/
struct newchrled_dev{
    dev_t devid;    //設備號
    int major;      //次設備號
    int minor;      //次設備號

};

struct newchrled_dev newchrled;  //led設備

/*入口*/
static int __init newchrled_init(void)
{
    int ret = 0;
    /*1. 初始化LED*/
    
    /*2. 註冊字符設備*/
    if(newchrled.major){ //給定主設備號
        newchrled.devid = MKDEV(newchrled.major, 0);
        ret = register_chrdev_region(newchrled.devid, NEWCHRLED_COUNT, NEWCHRLED_NAME);
    }
    else{                   //沒有給定主設備號
        ret = alloc_chrdev_region(&newchrled.devid, 0, NEWCHRLED_COUNT, NEWCHRLED_NAME);/* 申請一個設備號 */
        newchrled.major = MAJOR(newchrled.devid);                           /* 取出分配到的主號 */
        newchrled.minor = MINOR(newchrled.devid);                           /* 取出分配到的從號 */
    }
    if (ret < 0)
    {
        printk("newchrled chrdev_region err\r\n");
        return -1;
    }
    printk("newchrled major = %d, minor = %d",newchrled.major, newchrled.minor);

    return 0;
}
/*出口*/
static void __exit newchrled_exit(void)
{
    /*註銷字符設備*/
    unregister_chrdev_region(newchrled.devid, NEWCHRLED_COUNT);
}


/* 註冊和卸載驅動 */
module_init(newchrled_init);
module_exit(newchrled_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("a0000");