// dtsled.c — 以設備樹提供 GPIO 的 LED 字符驅動（支援自動閃爍）
// 兼容字串：mcalab,newchrled
// 需要 overlay 節點含有：label (可選), led-gpios
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>
#include <linux/jiffies.h>

#define DRV_NAME     "dtsled"
#define DEF_LABEL    "myled"

struct dtsled_dev {
    /* 平台/DT */
    struct device       *dev;
    struct gpio_desc    *gpiod;

    /* 字符設備 */
    dev_t                devid;
    struct cdev          cdev;
    struct device       *cdevice;
    char                 name[32];

    /* 狀態與鎖 */
    struct mutex         lock;
    int                  state;       /* 0/1 */

    /* 自動閃爍 */
    struct delayed_work  blink_work;
    bool                 blinking;
    unsigned int         on_ms;
    unsigned int         off_ms;
};

static struct class *dtsled_class;  /* 全域 class，供多個實例共用 */

/* 內部：設定 LED，需在持鎖下呼叫 */
static void __dtsled_set_locked(struct dtsled_dev *ld, int v)
{
    gpiod_set_value_cansleep(ld->gpiod, v);
    ld->state = !!v;
}

/* ========== 文件操作 ========== */
static ssize_t dtsled_read(struct file *f, char __user *buf, size_t len, loff_t *ppos)
{
    struct dtsled_dev *ld = f->private_data;
    char tmp[8];
    int n;

    mutex_lock(&ld->lock);
    n = scnprintf(tmp, sizeof(tmp), "%d\n", ld->state);
    mutex_unlock(&ld->lock);

    if (*ppos >= n) return 0;
    if (len < n) n = len;
    if (copy_to_user(buf, tmp, n)) return -EFAULT;
    *ppos += n;
    return n;
}

/* 支援：
 * "1" / "0" / "t"
 * "blink <on_ms> <off_ms>"
 * "blink_stop"
 * 手動寫入會打斷既有的自動閃爍（除非是 blink 指令）
 */
static ssize_t dtsled_write(struct file *f, const char __user *ubuf, size_t cnt, loff_t *ppos)
{
    struct dtsled_dev *ld = f->private_data;
    char kbuf[64];

    if (!cnt) return 0;
    if (cnt >= sizeof(kbuf)) cnt = sizeof(kbuf) - 1;
    if (copy_from_user(kbuf, ubuf, cnt)) return -EFAULT;
    kbuf[cnt] = '\0';

    /* 1) 自動閃爍：blink <on_ms> <off_ms> */
    if (!strncmp(kbuf, "blink ", 6)) {
        unsigned long on = 0, off = 0;
        if (sscanf(kbuf + 6, "%lu %lu", &on, &off) != 2 || on > 60000 || off > 60000)
            return -EINVAL;

        mutex_lock(&ld->lock);
        ld->on_ms  = (unsigned int)on;
        ld->off_ms = (unsigned int)off;
        ld->blinking = true;
        /* 依目前狀態排下一跳 */
        schedule_delayed_work(&ld->blink_work,
            msecs_to_jiffies(ld->state ? ld->on_ms : ld->off_ms));
        mutex_unlock(&ld->lock);
        return cnt;
    }

    /* 2) 停止自動閃爍：blink_stop */
    if (!strncmp(kbuf, "blink_stop", 10)) {
        bool do_cancel = false;

        mutex_lock(&ld->lock);
        if (ld->blinking) {
            ld->blinking = false;
            do_cancel = true;
        }
        mutex_unlock(&ld->lock);

        if (do_cancel)
            cancel_delayed_work_sync(&ld->blink_work);
        return cnt;
    }

    /* 3) 手動控制：1 / 0 / t(T) */
    if (kbuf[0] == '1' || kbuf[0] == '0' || kbuf[0] == 't' || kbuf[0] == 'T') {
        int newstate;
        bool do_cancel = false;

        mutex_lock(&ld->lock);
        if (ld->blinking) {
            ld->blinking = false;      /* 先關掉自動閃爍旗標 */
            do_cancel = true;          /* 等會兒到鎖外 cancel_sync */
        }

        if (kbuf[0] == '1')
            newstate = 1;
        else if (kbuf[0] == '0')
            newstate = 0;
        else /* toggle */
            newstate = !ld->state;

        __dtsled_set_locked(ld, newstate);
        mutex_unlock(&ld->lock);

        if (do_cancel)
            cancel_delayed_work_sync(&ld->blink_work);
        return cnt;
    }

    return -EINVAL;
}


static int dtsled_open(struct inode *ino, struct file *f)
{
    struct dtsled_dev *ld = container_of(ino->i_cdev, struct dtsled_dev, cdev);
    f->private_data = ld;
    return 0;
}

static int dtsled_release(struct inode *ino, struct file *f)
{
    return 0;
}

static const struct file_operations dtsled_fops = {
    .owner   = THIS_MODULE,
    .read    = dtsled_read,
    .write   = dtsled_write,
    .open    = dtsled_open,
    .release = dtsled_release,
    // .llseek  = noop_llseek,
};

/* ========== 閃爍 work ========== */
static void dtsled_blink_worker(struct work_struct *work)
{
    struct dtsled_dev *ld = container_of(to_delayed_work(work), struct dtsled_dev, blink_work);

    mutex_lock(&ld->lock);
    if (!ld->blinking) {
        mutex_unlock(&ld->lock);
        return;
    }
    __dtsled_set_locked(ld, !ld->state);
    schedule_delayed_work(&ld->blink_work,
        msecs_to_jiffies(ld->state ? ld->on_ms : ld->off_ms));
    mutex_unlock(&ld->lock);
}

/* ========== 平台驅動 ========== */
static int dtsled_probe(struct platform_device *pdev)
{
    struct dtsled_dev *ld;
    const char *label = NULL;
    int ret;

    ld = devm_kzalloc(&pdev->dev, sizeof(*ld), GFP_KERNEL);
    if (!ld) return -ENOMEM;

    ld->dev = &pdev->dev;
    mutex_init(&ld->lock);
    INIT_DELAYED_WORK(&ld->blink_work, dtsled_blink_worker);
    ld->on_ms = 500;
    ld->off_ms = 500;
    ld->blinking = false;
    ld->state = 0;

    of_property_read_string(pdev->dev.of_node, "label", &label);
    if (!label) label = DEF_LABEL;
    strscpy(ld->name, label, sizeof(ld->name));

    /* 從 DT 取得 "led-gpios"（consumer 名稱 "led"） */
    ld->gpiod = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
    if (IS_ERR(ld->gpiod)) {
        dev_err(&pdev->dev, "failed to get led-gpios\n");
        return PTR_ERR(ld->gpiod);
    }

    /* 分配/註冊字元設備 */
    ret = alloc_chrdev_region(&ld->devid, 0, 1, ld->name);
    if (ret) {
        dev_err(&pdev->dev, "alloc_chrdev_region: %d\n", ret);
        return ret;
    }

    cdev_init(&ld->cdev, &dtsled_fops);
    ld->cdev.owner = THIS_MODULE;
    ret = cdev_add(&ld->cdev, ld->devid, 1);
    if (ret) {
        dev_err(&pdev->dev, "cdev_add: %d\n", ret);
        unregister_chrdev_region(ld->devid, 1);
        return ret;
    }

    ld->cdevice = device_create(dtsled_class, NULL, ld->devid, NULL, "%s", ld->name);
    if (IS_ERR(ld->cdevice)) {
        ret = PTR_ERR(ld->cdevice);
        dev_err(&pdev->dev, "device_create: %d\n", ret);
        cdev_del(&ld->cdev);
        unregister_chrdev_region(ld->devid, 1);
        return ret;
    }

    platform_set_drvdata(pdev, ld);
    dev_info(&pdev->dev, "probed: /dev/%s ready\n", ld->name);
    return 0;
}

static void dtsled_remove(struct platform_device *pdev)
{
    struct dtsled_dev *ld = platform_get_drvdata(pdev);

    ld->blinking = false;
    cancel_delayed_work_sync(&ld->blink_work);

    device_destroy(dtsled_class, ld->devid);
    cdev_del(&ld->cdev);
    unregister_chrdev_region(ld->devid, 1);

}

static const struct of_device_id dtsled_of_match[] = {
    { .compatible = "mcalab,newchrled" },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, dtsled_of_match);

static struct platform_driver dtsled_driver = {
    .probe  = dtsled_probe,
    .remove = dtsled_remove,
    .driver = {
        .name           = DRV_NAME,
        .of_match_table = dtsled_of_match,
    },
};

/* 模組載入/卸載：先建立 class，再註冊 platform_driver */
static int __init dtsled_init(void)
{
    int ret;
    dtsled_class = class_create("newchrled");
    if (IS_ERR(dtsled_class)) {
        pr_err("class_create failed\n");
        return PTR_ERR(dtsled_class);
    }
    ret = platform_driver_register(&dtsled_driver);
    if (ret) {
        class_destroy(dtsled_class);
        return ret;
    }
    pr_info(DRV_NAME ": loaded\n");
    return 0;
}

static void __exit dtsled_exit(void)
{
    platform_driver_unregister(&dtsled_driver);
    class_destroy(dtsled_class);
    pr_info(DRV_NAME ": unloaded\n");
}

module_init(dtsled_init);
module_exit(dtsled_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("LED char device via DT (RPi5/BCM2712)");
