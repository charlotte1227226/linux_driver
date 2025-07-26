#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h> // make 前沒有裝 看教學打得
#include <linux/uaccess.h>

#define CHRDEVBASE_MAJOR       200              // 主設備號
#define CHRDEVBASE_NAME        "chrdevbase"     //名字

static char readbuf[100]; /*讀緩衝*/
static char writebuf[100]; /*寫緩衝*/
static char kerneldata[] = {"kernel data!"};

static int chrdevbase_open(struct inode *inode, struct file *filp)
{
    printk("chrdevbase_open\r\n");
    return 0;
}

static int chrdevbase_release(struct inode *inode, struct file *filp)
{
    printk("chrdevbase_release\r\n");
    return 0;
}
/*release就是close*/

static ssize_t chrdevbase_read(struct file *filp, __user char *buf, size_t count, loff_t *ppos)
{
    int ret = 0;
    //printk("chrdevbase_read\r\n");
    memcpy(readbuf, kerneldata, sizeof(kerneldata)); // void * memcpy(void *dest, const void *src, size_t n);
    ret = copy_to_user(buf, readbuf, count); // 把read_buf的資料放到buf 讀count個
    if (ret == 0) {

    }
    else {

    }
    return 0;
}

static ssize_t chrdevbase_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
    //printk("chrdevbase_write\r\n");
    int ret = 0;
    ret = copy_from_user(writebuf, buf, count);
    if (ret == 0) {
        printk("kernel recevdata:%s\r\n", writebuf);
    }
    else {
        
    }
    return 0;
}

static struct file_operations chrdevbase_fops = {
    .owner = THIS_MODULE,
    .open = chrdevbase_open,
    .release = chrdevbase_release,
    .read = chrdevbase_read,
    .write = chrdevbase_write,
};
/*實現設備所需的函數*/

/*
字符設備操作合集
*/


static int __init chrdev_init(void)
{
    int ret = 0;
    printk("chrdevbase_init\r\n");

    /*註冊字符設備*/
    ret = register_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME, &chrdevbase_fops);
    if(ret < 0){
        printk("chrdevbase init failed!\r\n");
    }
    return 0;
}
static void __exit chrdev_exit(void)
{
    printk("chrdevbese_exit\r\n");

    /*註銷字符設備*/
    unregister_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME);
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