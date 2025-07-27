# RP1 register

---

## ✅ RP1 GPIO 主要暫存器地址總表

GPIO peripheral base address：

```c
#define GPIO_BASE  0x60004000
```

以下是根據 RP1 文件彙整出來的 GPIO 暫存器列表（偏移以 byte 為單位）：

---

### 🔧 Function Select（設定 GPIO 為輸入/輸出/ALT）

| 暫存器名稱   | 地址偏移 | 說明         |
| ------- | ---- | ---------- |
| GPFSEL0 | 0x00 | GPIO 0–9   |
| GPFSEL1 | 0x04 | GPIO 10–19 |
| GPFSEL2 | 0x08 | GPIO 20–29 |
| GPFSEL3 | 0x0C | GPIO 30–39 |
| GPFSEL4 | 0x10 | GPIO 40–49 |
| GPFSEL5 | 0x14 | GPIO 50–57 |

---

### 🔧 Output Control（設為高/低）

| 暫存器名稱  | 地址偏移 | 說明                       |
| ------ | ---- | ------------------------ |
| GPSET0 | 0x1C | GPIO 0–31 拉高（set to 1）   |
| GPSET1 | 0x20 | GPIO 32–57 拉高            |
| GPCLR0 | 0x28 | GPIO 0–31 拉低（clear to 0） |
| GPCLR1 | 0x2C | GPIO 32–57 拉低            |

---

### 🔧 Input Read（讀 GPIO 電位）

| 暫存器名稱  | 地址偏移 | 說明            |
| ------ | ---- | ------------- |
| GPLEV0 | 0x34 | GPIO 0–31 狀態  |
| GPLEV1 | 0x38 | GPIO 32–57 狀態 |

---

### 🔧 Pull-up / Pull-down 控制

| 暫存器名稱                       | 地址偏移 | 說明                   |
| --------------------------- | ---- | -------------------- |
| GPIO\_PUP\_PDN\_CNTRL\_REG0 | 0xE4 | GPIO 0–15（每腳 2 bits） |
| GPIO\_PUP\_PDN\_CNTRL\_REG1 | 0xE8 | GPIO 16–31           |
| GPIO\_PUP\_PDN\_CNTRL\_REG2 | 0xEC | GPIO 32–47           |
| GPIO\_PUP\_PDN\_CNTRL\_REG3 | 0xF0 | GPIO 48–57           |

* `00`：無拉電阻
* `01`：Pull-Down
* `10`：Pull-Up

---

### 進階（如你需要）

| 暫存器名稱                | 地址偏移         | 說明                            |
| -------------------- | ------------ | ----------------------------- |
| GPEDSn               | 0x40/0x44    | Event Detect Status           |
| GPRENn               | 0x4C/0x50    | Rising Edge Detect Enable     |
| GPFENn               | 0x58/0x5C    | Falling Edge Detect Enable    |
| GPIO\_PADS\_BANK0\~2 | 0x300\~0x308 | Drive strength/Slew rate（進階用） |

---

## 🧾 範例定義（建議寫法）

```c
#define GPIO_BASE     0x60004000
#define GPFSEL1       (*(volatile uint32_t *)(GPIO_BASE + 0x04))
#define GPSET0        (*(volatile uint32_t *)(GPIO_BASE + 0x1C))
#define GPCLR0        (*(volatile uint32_t *)(GPIO_BASE + 0x28))
#define GPIO_PUP_PDN_CNTRL_REG1 (*(volatile uint32_t *)(GPIO_BASE + 0xE8))  // for GPIO17
```

---
| 你寫的                                              | 實際意思      | 有效果嗎？   |
| ------------------------------------------------ | --------- | ------- |
| `GPIO_BASE + 0x1C`                               | 單純算出地址    | ❌ 沒操作硬體 |
| `(volatile uint32_t *)(GPIO_BASE + 0x1C)`        | 是指標，但還沒動手 | ❌       |
| `*(volatile uint32_t *)(GPIO_BASE + 0x1C)`       | 有動手，讀暫存器  | ✅（讀取）   |
| `*(volatile uint32_t *)(GPIO_BASE + 0x1C) = val` | 有動手，寫暫存器  | ✅（寫入）   |

---

很好，我來幫你詳細說明你剛剛看到的那些 GPIO 暫存器是什麼意思，這些都是你寫 driver 或裸機程式時必須操作的硬體暫存器。以下是依功能分類說明：

---

## 🔧 一、GPFSELn（Function Select Registers）

**目的：設定每個 GPIO 腳的功能（輸入 / 輸出 / 特殊功能）**

* 每個 GPIO 佔 3 個 bits
* 常見值：

  * `000`：Input（輸入模式）
  * `001`：Output（輸出模式）
  * 其他是 Alternate Functions（SPI, I2C...）

| 暫存器名稱   | 控制哪些 GPIO  |
| ------- | ---------- |
| GPFSEL0 | GPIO 0–9   |
| GPFSEL1 | GPIO 10–19 |
| GPFSEL2 | GPIO 20–29 |
| ...     | ...        |

🟩 **範例**：把 GPIO17 設成輸出（在 GPFSEL1 第 21\~23 位）

```c
GPFSEL1 &= ~(0b111 << 21); // 清除
GPFSEL1 |=  (0b001 << 21); // 設為輸出
```

---

## 🔧 二、GPSETn / GPCLRn（Set/Clear Registers）

**目的：讓 GPIO 輸出高電位（1）或低電位（0）**

* GPSETn：對應 bit 寫 1 → 該 GPIO 拉高
* GPCLRn：對應 bit 寫 1 → 該 GPIO 拉低

| 暫存器名稱  | 控制哪些 GPIO  |
| ------ | ---------- |
| GPSET0 | GPIO 0–31  |
| GPSET1 | GPIO 32–57 |
| GPCLR0 | GPIO 0–31  |
| GPCLR1 | GPIO 32–57 |

🟩 **範例**：讓 GPIO17 亮燈（輸出高電位）

```c
GPSET0 = (1 << 17); // GPIO17 拉高
GPCLR0 = (1 << 17); // GPIO17 拉低
```

---

## 🔧 三、GPLEVn（Level Register）

**目的：讀取目前 GPIO 腳的電位（1：高、0：低）**

| 暫存器名稱  | 代表 GPIO    |
| ------ | ---------- |
| GPLEV0 | GPIO 0–31  |
| GPLEV1 | GPIO 32–57 |

🟩 **範例**：讀 GPIO17 的狀態

```c
int value = (GPLEV0 & (1 << 17)) != 0;
```

---

## 🔧 四、GPIO\_PUP\_PDN\_CNTRL\_REGn（Pull-Up / Pull-Down 控制）

**目的：設定 GPIO 腳內部是否接上拉電阻（pull-up）或下拉電阻（pull-down）**

* 每個 GPIO 用 2 個 bits 控制：

  * `00`：無拉電阻（High-Z）
  * `01`：Pull-down
  * `10`：Pull-up
  * `11`：保留

| 暫存器名稱       | 控制哪一組 GPIO |
| ----------- | ---------- |
| CNTRL\_REG0 | GPIO 0–15  |
| CNTRL\_REG1 | GPIO 16–31 |
| CNTRL\_REG2 | GPIO 32–47 |
| CNTRL\_REG3 | GPIO 48–57 |

🟩 **範例**：把 GPIO17 設成無拉電阻

```c
GPIO_PUP_PDN_CNTRL_REG1 &= ~(0b11 << (17 * 2 - 32)); // 先清掉
GPIO_PUP_PDN_CNTRL_REG1 |=  (0b00 << (17 * 2 - 32)); // 設為 00
```

---

## 🔧 五、其他進階暫存器（可選）

| 名稱                | 用途                                     |
| ----------------- | -------------------------------------- |
| GPEDSn            | 偵測是否發生事件                               |
| GPRENn            | 上升緣偵測使能                                |
| GPFENn            | 下降緣偵測使能                                |
| GPIO\_PADS\_BANKx | 設定 drive strength、slew rate 等電氣特性（進階用） |

如果你只是單純要點亮 LED，前面四組就夠了。

---

## ✅ 小結

| 操作    | 暫存器                       | 做什麼       |
| ----- | ------------------------- | --------- |
| 設成輸出  | `GPFSELn`                 | 設定為 `001` |
| 輸出高/低 | `GPSETn / GPCLRn`         | 控制燈亮/滅    |
| 讀取輸入  | `GPLEVn`                  | 讀 pin 狀態  |
| 拉電阻   | `GPIO_PUP_PDN_CNTRL_REGn` | 設內部上拉/下拉  |

---
`volatile` 是 C/C++ 語言中的一個**關鍵字修飾符（type qualifier）**，在嵌入式程式中**非常重要**，尤其是在你現在要操作 GPIO 的情境中更是不可或缺。

---

## ✅ `volatile` 的意義

```c
volatile uint32_t *ptr = (uint32_t *)0x60004000;
```

這代表：

> **這個變數所指向的記憶體內容可能會「隨時改變」，不可以讓編譯器做最佳化。**

---

### 🔧 一般情況下的編譯器行為（沒有 volatile）：

```c
uint32_t *reg = (uint32_t *)0x60004000;
uint32_t value = *reg;   // 讀一次
value = *reg;            // 編譯器可能優化成跳過這行（因為值沒變）
```

這對**記憶體-mapped 的硬體暫存器是錯誤的**！

因為：

* 暫存器的內容可能是硬體控制邏輯在改（GPIO 狀態、UART flag、ADC 值等）
* 編譯器如果幫你省略讀寫，就看不到真實狀態了。

---

## ✅ 為什麼硬體暫存器要用 `volatile`？

```c
#define GPSET0 (*(volatile uint32_t *)(GPIO_BASE + 0x1C))
```

這樣宣告後，每次讀/寫 `GPSET0`，都會**強制從記憶體中真正讀取或寫入**，不會被快取或優化。

---

## ✅ 舉例比較

| 寫法                                         | 結果          |
| ------------------------------------------ | ----------- |
| `*(uint32_t *)0x60004000 = 0x01;`          | 可能被最佳化或略過   |
| `*(volatile uint32_t *)0x60004000 = 0x01;` | 編譯器一定寫入這個地址 |

---

## ✅ 總結

| 項目             | 說明                                         |
| -------------- | ------------------------------------------ |
| `volatile` 是什麼 | 告訴編譯器「不要對這個變數做優化」                          |
| 什麼時候用          | 操作 GPIO、UART、ADC、Timer、Interrupt 等「硬體暫存器」時 |
| 不加會怎樣          | 程式可能讀不到變化，或根本沒寫入暫存器，導致 LED 不亮、資料不傳等錯誤行為    |

---
# GPIO17設置LED
每顆 GPIO 在 GPFSEL 寄存器裡都是用 **3 個連續的 bits** 來設定模式（000 = input, 001 = output, …）。

1. **哪個 GPFSEL？**

   * GPIO0–GPIO9 → GPFSEL0
   * GPIO10–GPIO19 → GPFSEL1 ← 我們用的是 GPFSEL1

2. **算出 GPIO17 在 GPFSEL1 的「欄位位置」**

   * GPIO17 在這個寄存器是第 `(17 − 10) = 7` 欄（因為 GPFSEL1 控 GPIO10 起算）
   * 每欄佔 3 bits → 起始位址就是 `7 × 3 = 21`，覆蓋 bit21、22、23

3. **為什麼要 `~(0x7 << 21)`？**

   * `0x7` 二進位是 `0b111` → 正好 3 個 bits
   * `(0x7 << 21)` 就是把這 3 個連續的 1 移到位21–23 → `0b111000…0`
   * `~(...)` 反轉，變成 bit21–23 全是 0，其他都是 1 → 這樣用 AND，就只清掉那 3 bits，不動其他位元

4. **完整清＋設流程**

   ```c
   u32 val = readl(GPFSEL1_ptr);
   // 清除 bit21~23（也就是把那 3 個 bits 變 000）
   val &= ~(0x7 << 21);
   // 設成 001 (output 模式)
   val |=  (0x1 << 21);
   writel(val, GPFSEL1_ptr);
   ```

**關鍵公式**：

> 清除： `val &= ~(0x7 << ( (GPIO#%10) * 3 ));`
> 設定： `val |=  (mode  << ( (GPIO#%10) * 3 ));`

* `GPIO#%10` → 在這個 GPFSEL 裡是第幾欄
* 乘以 3 → 起始 bit 位址
* `0x7` → 3-bit 欄位全 1，用來清除
* 再用 `mode`（如 1 = output）去設定那三個 bits

這就是為什麼要用 `~(0x7 << 21)` 來「清掉先前的設定」──保證只影響 GPIO17 那一個欄位，其他 GPIO 的設定不會被碰掉。

---
# GPIO17完整設置

下面範例示範如何在 Linux kernel driver 裡，完整地設定並控制一組 GPIO（以 GPIO17 為例）。包括：

1. **映射整個 GPIO 區塊**
2. **定義各寄存器位址與位元**
3. **設定 pin 模式（input/output）**
4. **控制輸出高/低**
5. **設定上下拉（PUD）**
6. **釋放映射**

---

```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/delay.h>

#define LED_MAJOR   200
#define LED_NAME    "led"

// RP1（BCM2711）GPIO block physical base
#define GPIO_BASE_PHYS        0x6000 4000UL   // RP5 範例，RP1 對應是 0x2020_0000
#define GPIO_BLOCK_SIZE       SZ_4K

// 各 GPIO 寄存器 offset
#define GPFSEL0_OFS           0x00  // controls GPIO 0–9
#define GPFSEL1_OFS           0x04  // controls GPIO10–19
#define GPSET0_OFS            0x1C  // set GPIO 0–31 to 1
#define GPCLR0_OFS            0x28  // clear GPIO 0–31 to 0
#define GPLEV0_OFS            0x34  // read-level GPIO 0–31
#define GPPUD_OFS             0xE4  // pull-up/down control
#define GPPUDCLK0_OFS         0xE8  // pull-up/down clock for GPIO 0–31

// 我們要用的那顆 LED
#define LED_GPIO              17
#define LED_FSEL_REG_OFS      GPFSEL1_OFS
#define LED_FSEL_SHIFT        ((LED_GPIO - 10) * 3)  // 7*3 = 21
#define LED_FSEL_MASK         (0x7 << LED_FSEL_SHIFT)
#define LED_FSEL_OUTPUT       (0x1 << LED_FSEL_SHIFT)

// pull-up/down 控制
#define PUD_OFF               0x0
#define PUD_DOWN              0x1
#define PUD_UP                0x2

static void __iomem *gpio_base;

// 快捷宏：從 base + offset 讀/寫
#define GPIO_READ(ofs)        readl(gpio_base + (ofs))
#define GPIO_WRITE(ofs, v)    writel((v), gpio_base + (ofs))

static int led_open(struct inode *inode, struct file *filp)
{
    u32 v;

    // 1) 設成 output
    v = GPIO_READ(LED_FSEL_REG_OFS);
    v &= ~LED_FSEL_MASK;          // 清除原先的 3 bits
    v |= LED_FSEL_OUTPUT;         // 設為 001 (output)
    GPIO_WRITE(LED_FSEL_REG_OFS, v);

    // 2) 取消上下拉（off）
    GPIO_WRITE(GPPUD_OFS, PUD_OFF);
    udelay(5);
    GPIO_WRITE(GPPUDCLK0_OFS, (1 << LED_GPIO));
    udelay(5);
    GPIO_WRITE(GPPUDCLK0_OFS, 0);

    return 0;
}

static int led_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t led_write(struct file *filp,
                         const char __user *buf,
                         size_t count,
                         loff_t *ppos)
{
    char k;
    u32 bit = (1 << LED_GPIO);

    if (count < 1) return -EINVAL;
    if (copy_from_user(&k, buf, 1)) return -EFAULT;

    if (k == '1') {
        // set to HIGH
        GPIO_WRITE(GPSET0_OFS, bit);
    } else {
        // set to LOW
        GPIO_WRITE(GPCLR0_OFS, bit);
    }
    return 1;
}

static struct file_operations led_fops = {
    .owner   = THIS_MODULE,
    .open    = led_open,
    .release = led_release,
    .write   = led_write,
};

static int __init led_init(void)
{
    int ret;

    // 1) 映射整個 GPIO block
    gpio_base = ioremap(GPIO_BASE_PHYS, GPIO_BLOCK_SIZE);
    if (!gpio_base) {
        pr_err("LED: ioremap failed\n");
        return -ENOMEM;
    }

    // 2) 註冊字符設備
    ret = register_chrdev(LED_MAJOR, LED_NAME, &led_fops);
    if (ret < 0) {
        pr_err("LED: register_chrdev failed\n");
        iounmap(gpio_base);
        return ret;
    }

    pr_info("LED driver loaded\n");
    return 0;
}

static void __exit led_exit(void)
{
    unregister_chrdev(LED_MAJOR, LED_NAME);
    iounmap(gpio_base);
    pr_info("LED driver unloaded\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("a0000");
MODULE_DESCRIPTION("RP1 GPIO LED Driver Example");
```

---

#### 詳細說明

1. **`ioremap(GPIO_BASE_PHYS, SZ_4K)`**
   將整個 4 KiB GPIO 地址空間映射到內核，再用 `gpio_base + offset` 存取。

2. **GPFSELx（Function Select）**

   * 每顆 GPIO 用 3 bit 設定：
     − `000`：input
     − `001`：output
     − `100`…其他特殊功能
   * GPIO17 屬於 GPFSEL1，因為 GPFSEL1 控 `GPIO10–19`。
   * 欄位索引 = `(17 − 10) = 7`，3 bits 起始位址 = `7 × 3 = 21`。

3. **上下拉（Pull-up/down）**

   * 先寫 `GPPUD` 設置「off/down/up」。
   * 等 5 μs → 再在 `GPPUDCLK0` 置入對應引腳位元 → 等 5 μs → 清 `GPPUDCLK0`。

4. **輸出高/低**

   * `GPSET0` 寫 `1<<n` → 把第 n 顆 GPIO 拉高。
   * `GPCLR0` 寫 `1<<n` → 把第 n 顆 GPIO 拉低。

5. **卸載時**

   * `iounmap()` 解除映射，
   * `unregister_chrdev()` 注銷。

這樣就完成了從「映射」→「設定功能」→「設定上下拉」→「切換輸出」→「解除映射」的一整套 GPIO 驅動流程。
