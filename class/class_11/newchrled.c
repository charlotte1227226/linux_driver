#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>    /* struct cdev, cdev_init/add/del */
#include <linux/fs.h> 
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/device.h>

#define NEWCHRLED_NAME "newchrled"
#define NEWCHRLED_COUNT 1

#define LEDOFF  0   //關閉
#define LEDON   1   //打開

/*LED 設備結構體*/
struct newchrled_dev{
    struct cdev cdev;       //字符設備
    dev_t devid;            //設備號
    struct class *class;    //類別
    struct device *device;  //設備
    int major;              //次設備號
    int minor;              //次設備號

};

struct newchrled_dev newchrled;  //led設備

static int newchrled_open(struct inode *inode, struct file *filp)
{
    return 0;
}

static int newchrled_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t newchrled_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
    return 0;
}

static const struct file_operations newchrled_fops = {
    .owner = THIS_MODULE,
    .write = newchrled_write,
    .open = newchrled_open,
    .release = newchrled_release,
};
/*入口*/
static int __init newchrled_init(void)
{
    int ret = 0;
    printk("newchrled_init\r\n");
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
    printk("newchrled major = %d, minor = %d\r\n",newchrled.major, newchrled.minor);

    newchrled.cdev.owner = THIS_MODULE;
    cdev_init(&newchrled.cdev, &newchrled_fops);
    ret = cdev_add(&newchrled.cdev, newchrled.devid, NEWCHRLED_COUNT);

    /*3. 自動創建設備節點*/
    /*新增類別*/
    // newchrled.class = class_create(THIS_MODULE, NEWCHRLED_NAME);
    newchrled.class = class_create(NEWCHRLED_NAME); 
    //編譯時struct class * __must_check class_create(const char *name);
    //傳一個參數就好
    if (IS_ERR(newchrled.class)) 
    {
        return PTR_ERR(newchrled.class);
    }
    /*新增設備*/
    newchrled.device = device_create(newchrled.class, NULL, newchrled.devid, NULL, NEWCHRLED_NAME);
    if (IS_ERR(newchrled.device)) 
    {
        return PTR_ERR(newchrled.device);
    }
    return 0;
}
/*出口*/
static void __exit newchrled_exit(void)
{
    printk("newchrled_exit\r\n");
    /*刪除字符設備*/
    cdev_del(&newchrled.cdev);
    /*註銷字符設備*/
    unregister_chrdev_region(newchrled.devid, NEWCHRLED_COUNT);
    /*刪除設備節點*/
    /*摧毀設備*/
    device_destroy(newchrled.class, newchrled.devid);
    /*摧毀類別*/
    class_destroy(newchrled.class);
    
}


/* 註冊和卸載驅動 */
module_init(newchrled_init);
module_exit(newchrled_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("a0000");