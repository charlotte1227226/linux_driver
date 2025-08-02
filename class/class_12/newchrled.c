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
    filp->private_data = &newchrled;
    return 0;
}

static int newchrled_release(struct inode *inode, struct file *filp)
{
    struct newchrled_dev *dev = (struct newchrled_dev *)filp->private_data;
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
    int result;
    printk("newchrled_init\r\n");
    /*1. 初始化LED*/
    
    newchrled.major = 0; //未主動(手動)配置主設備號，手動清成零，設置為零，表是由系統申請設備號
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
        // return -1;
        goto fail_devid;
    }
    printk("newchrled major = %d, minor = %d\r\n",newchrled.major, newchrled.minor);

    newchrled.cdev.owner = THIS_MODULE;
    // 初始化 cdev 結構
    /* cdev_init() 會把你的 newchrled_fops（包含 .open/.read/.write…）掛到 newchrled.cdev 裡，
    並做好內部初值設定 
    */
    cdev_init(&newchrled.cdev, &newchrled_fops);
    /* cdev_add() 會把這個 struct cdev 跟你先前透過
     alloc_chrdev_region() 或 register_chrdev_region() 
     拿到的 dev_t newchrled.devid（major/minor）連結起來，
     並建立 /proc/devices、/sys/class/... 等其他核心結構。
    */
    // 把 cdev 加到系統
    ret = cdev_add(&newchrled.cdev, newchrled.devid, NEWCHRLED_COUNT);
    if (ret < 0)
    {
        printk("fail_cdev_add\r\n");
        goto fail_cdev_add;
    }
    /*3. 自動創建設備節點*/
    /*新增類別*/
    // newchrled.class = class_create(THIS_MODULE, NEWCHRLED_NAME);
    newchrled.class = class_create(NEWCHRLED_NAME); 
    //編譯時struct class * __must_check class_create(const char *name);
    //傳一個參數就好
    if (IS_ERR(newchrled.class)) 
    {
        ret = PTR_ERR(newchrled.class);
        printk("fail_class\r\n");
        // return PTR_ERR(newchrled.class);
        goto fail_class;
    }
    /*新增設備*/
    newchrled.device = device_create(newchrled.class, NULL, newchrled.devid, NULL, NEWCHRLED_NAME);
    if (IS_ERR(newchrled.device)) 
    {
        ret = PTR_ERR(newchrled.device);
        printk("fail_class\r\n");
        // return PTR_ERR(newchrled.device);
        goto fail_device;
    }
    return 0; //正常結束


//由下往上(最後階段->倒數第2階段->...->第2階段->第1階段)依序註銷
//類似switch 沒有return 或break會一直向下執行
fail_device:
    class_destroy(newchrled.class); // 摧毀類別
fail_class:
    cdev_del(&newchrled.cdev); // 刪除字符設備
fail_cdev_add:
    unregister_chrdev_region(newchrled.devid, NEWCHRLED_COUNT); // 註銷字符設備
fail_devid:
    return -ret; //第1階段錯誤

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