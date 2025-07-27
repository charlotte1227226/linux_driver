#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h> 
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/types.h>

#define LED_MAJOR   200
#define LED_NAME    "rpi_led"

// Device-tree 讀到的 PIO 區段（PL061）的物理 base/size
#define GPIO_PIO_BASE_HI   0xC0000000UL   /* 高 32 bits */
#define GPIO_PIO_BASE_LO   0x00801740UL   /* 低 32 bits */
#define GPIO_PIO_SIZE      0x1000         /* 一頁 4 KiB */


/* 把 64-bit base 對齊到頁邊界（4 KiB） */
#define GPIO_BASE_PHYS  (((u64)GPIO_PIO_BASE_HI << 32 | GPIO_PIO_BASE_LO) & ~0xFFFULL)

#define PIO_SIZE        GPIO_PIO_SIZE

/* GPIO17 的 PIO block 實體位址（須自行對齊到頁邊界） */
// #define GPIO_BASE_PHYS    0xC0801740UL
// 先用 (GPIO_BASE & ~0xFFF) 取得 0xC0801000，再 ioremap(PIO_SIZE)

/* 以下偏移都是「從 GPIO_BASE 算起」的位移量 */
#define GPFSEL1_OFFSET        0x04    /* GPFSEL1: 設定 GPIO 10–19 為 input/output */
#define GPSET0_OFFSET         0x1C    /* GPSET0: 〈1〉把 GPIO0–31 對應位元設為 1（set）*/
#define GPCLR0_OFFSET         0x28    /* GPCLR0: 〈0〉把 GPIO0–31 對應位元設為 0（clear）*/
#define GPPUD_OFFSET          0xE4    /* GPPUD: Pull-Up/Down 控制（0=off,1=down,2=up）*/
#define GPPUDCLK0_OFFSET      0xE8    /* GPPUDCLK0: clock 哪些 GPIO 腳，把 pull 設定套用到對應腳 */

static void __iomem *pio_base;  // 一次映射整個 PIO block

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

static int __init led_init(void)
{
    int ret;
    u32 val;

    /* 1. 初始化 LED燈， 地址映射：一次把 4 KiB 全部映射進來 */
    pio_base = ioremap(GPIO_BASE_PHYS, PIO_SIZE);
    if (!pio_base) {
        pr_err("Failed to ioremap PIO block\n");
        return -ENOMEM;
    }

    /* 2. 初始化功能：設 GPIO17 (位 17) 為輸出 */
    val = readl(pio_base + GPFSEL1_OFFSET);
    val &= ~(0x7 << 21);
    // 清除 GPIO17 先前的功能設定（3 bits）
    // (0x7 << 21)==000111...000，~(...) = 111000...111
    // 與 AND 後會把 GPIO17 那組 bits 清成 0

    val |= (0x1 << 21);
    // xxx001xxx... = GPIO17 output 模式
    writel(val, pio_base + GPFSEL1_OFFSET);

    /* 如果你要打上 Pull-Up / Pull-Down，一般要：
     *  1) 寫 GPPUD 選 0(off)/1(pulldown)/2(pullup)
     *  2) udelay(5)
     *  3) 寫 GPPUDCLK0 把 clock 打到要設定的腳位 (1<<17)
     *  4) udelay(5)
     *  5) 寫 GPPUD 回 0
     *  6) udelay(5)
     *  7) 寫 GPPUDCLK0 回 0
     *  8) udelay(5)
     */
    writel(2, pio_base + GPPUD_OFFSET);            // 我們這裡舉例要開 Pull-Up
    udelay(5);
    writel(1 << 17, pio_base + GPPUDCLK0_OFFSET);  // GPIO17 引腳 clock 打開
    udelay(5);
    writel(0, pio_base + GPPUD_OFFSET);            // 清回 0
    udelay(5);
    writel(0, pio_base + GPPUDCLK0_OFFSET);        // 清回 0
    udelay(5);

    /* 拉高 GPIO17（LED on） */
    writel(1 << 17, pio_base + GPSET0_OFFSET);

    /* 或者拉低 GPIO17（LED off）：
     * writel(1 << 17, pio_base + GPCLR0_OFFSET);
     */

    /* 註冊字符設備 */
    ret = register_chrdev(LED_MAJOR, LED_NAME, &led_fops);
    if (ret < 0) {
        pr_err("register_chrdev failed: %d\n", ret);
        iounmap(pio_base);
        return ret;
    }

    pr_info("rpi_led inited\n");
    return 0;
}

static void __exit led_exit(void)
{
    /* 取消地址映射 */
    iounmap(pio_base);

    /* 註銷字符設備 */
    unregister_chrdev(LED_MAJOR, LED_NAME);
    pr_info("rpi_led exited\n");
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("a0000");
MODULE_DESCRIPTION("RP1 GPIO17 LED Driver");