# linux driver在rasberry pi 5 所需的環境測試

# 一、背景與需求

1. **目標**：在樹莓派（Raspberry Pi）上「動手實作」一支最簡單的 Linux 字元設備驅動（`hello_char`），並在 user-space 用程式讀取 `/dev/hello_char`。
2. **關鍵挑戰**：

   * 樹莓派 OS 預設沒有完整的 Kernel headers／原始碼，無法直接編譯外部 (out-of-tree) 模組。
   * 取得並準備「對應目前 kernel 版本」（`uname -r` → `6.12.25+rpt-rpi-2712`）的開發樹。
   * 解決編譯過程中缺少 `autoconf.h`、`auto.conf`、`Module.symvers`、OpenSSL headers、flex/bison 等問題。
   * 簡化最終流程，只要在驅動專案目錄下一條 `make` 即可完成編譯。

---

# 二、環境檢查與初步準備

1. **確認 kernel headers 是否已安裝**

   ```bash
   dpkg -l | grep raspberrypi-kernel-headers  
   # 已安裝 → ii  raspberrypi-kernel-headers 1:1.20230405-1 arm64
   ls /usr/src/linux-headers-$(uname -r)/include/linux/fs.h  
   # 發現：No such file or directory → headers 版本與 kernel 版本不符
   ```
2. **嘗試使用 APT 原始碼包**

   * 啟用 `deb-src` 行、加入 Raspberry Pi OS source repo
   * `apt update` → `apt source raspberrypi-kernel` → 實際下載到的是 **raspberrypi-firmware**，並非 kernel 源碼

---

# 三、取得正確的 Kernel 原始碼

1. **從 GitHub clone**

   ```bash
   cd ~/kernel-src
   git clone --depth=1 -b rpi-6.12.y https://github.com/raspberrypi/linux.git
   ```

   * `--depth=1`：只抓最新提交
   * `-b rpi-6.12.y`：對應 6.12.x 的分支

2. **建立 `/lib/modules/.../build` 符號連結**

   ```bash
   sudo rm -f /lib/modules/$(uname -r)/build
   sudo ln -s ~/kernel-src/linux /lib/modules/$(uname -r)/build
   ```

   * 讓內核的 build system（`make -C /lib/modules/...`）去讀你剛 clone 的原始碼樹

---

# 四、撰寫 & 放置驅動原始碼

1. **在桌面建立專案結構**

   ```
   ~/Desktop/linux_driver/
   └── char_demo/
       ├── hello_char.c   ← Kernel module 程式
       └── Makefile       ← 外部模組編譯腳本
   ```
2. **`hello_char.c`**：簡單實作 `.open`、`.read`、`.release`、`register_chrdev`

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>    // 新增這行
#define DEVICE_NAME "hello_char"
static int major;
// open
static int hello_open(struct inode *inode, struct file *filp) {
    pr_info("hello_char: open()\n");
    return 0;
}
// read
static ssize_t hello_read(struct file *filp, char __user *buf, size_t len, loff_t *offset) {
    const char *msg = "Hello from PI!\n";
    size_t msg_len = strlen(msg);

    if (*offset >= msg_len) return 0;             // EOF
    if (len > msg_len - *offset) len = msg_len - *offset;

    if (copy_to_user(buf, msg + *offset, len)) return -EFAULT;
    *offset += len;
    return len;
}
// release
static int hello_release(struct inode *inode, struct file *filp) {
    pr_info("hello_char: release()\n");
    return 0;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = hello_open,
    .read    = hello_read,
    .release = hello_release,
};

static int __init hello_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("hello_char: register_chrdev failed\n");
        return major;
    }
    pr_info("hello_char: registered with major %d\n", major);
    return 0;
}

static void __exit hello_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("hello_char: unregistered\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("a0000");
MODULE_DESCRIPTION("Simple Hello Char Driver");
```
3. **`Makefile`**：指向 `/lib/modules/$(uname -r)/build`，並啟用外部符號表

   ```makefile
   KBUILD_EXTRA_SYMBOLS := /home/a0000/kernel-src/linux/Module.symvers

   obj-m += hello_char.o

   KDIR := /lib/modules/$(shell uname -r)/build
   PWD  := $(shell pwd)

   all:
   	$(MAKE) -C $(KDIR) M=$(PWD) modules

   clean:
   	$(MAKE) -C $(KDIR) M=$(PWD) clean
   ```

---

# 五、準備 Kernel Tree（生成必要檔案）

1. **複製系統用的 `.config`**

   ```bash
   cd ~/kernel-src/linux
   cp /boot/config-$(uname -r) .config
   ```
2. **安裝必需工具**

   ```bash
   sudo apt install libssl-dev flex bison
   ```
3. **自動填充新選項**

   ```bash
   make olddefconfig
   ```
4. **生成 out-of-tree module 所需檔案**

   ```bash
   make -j$(nproc) modules_prepare
   ```
   或是
    ```bash
   make -j$(nproc) modules
   ```
   * 產生：

     * `include/generated/autoconf.h`
     * `include/config/auto.conf`
     * 各種 header wrapping、scripts/mod/modpost 等
5. **快速生成 `Module.symvers`**（只要符號表）

   ```bash
   scripts/mod/modpost -m Module.symvers
   ```

   * 不需編譯所有模組 → 幾十秒內完工
   * `Module.symvers` 收錄所有導出符號及版本資訊

---

# 六、編譯、載入與測試驅動

1. **切回驅動專案**

   ```bash
   cd ~/Desktop/linux_driver/char_demo
   make clean && make
   ```

   * `Makefile` 已內建 `KBUILD_EXTRA_SYMBOLS` → `modpost` 不再報 undefined
   * 輸出：`hello_char.ko`
下面我逐行用繁體中文說明這段測試流程的每一步及其背後的原理：

```bash
# 1. 卸載舊模組（如果已經載入的話）
sudo rmmod hello_char
```

* **作用**：移除（remove）已經載入到核心中的 `hello_char` 模組。
* **為什麼要做**：確保接下來載入的是你剛剛重新編譯的最新版本，不會因為核心中已經有舊版本而造成衝突。

```bash
# 2. 刪除舊的節點檔案
sudo rm -f /dev/hello_char
```

* **作用**：刪除先前可能已建立的 `/dev/hello_char` 裝置節點。
* **為什麼要做**：避免舊的節點仍指向舊的 major/minor 或被錯誤權限鎖住，確保之後建立的節點是正確的。

```bash
# 3. 重新插入你的模組
sudo insmod hello_char.ko
#    確認 dmesg 上顯示的是 major=509
dmesg | tail -n2
```

* **`sudo insmod hello_char.ko`**：將剛編譯出的 `hello_char.ko` 模組插入到 Linux 核心。
* **核心行為**：執行模組的 `module_init` 函式（`hello_init`），呼叫 `register_chrdev` 並向核心申請一個新的 major 編號。
* **`dmesg | tail -n2`**：查看系統日誌最後兩行，尋找模組註冊時印出的訊息，例如：

  ```
  hello_char: registered with major 509
  ```
* **為什麼要做**：拿到系統分配給你的「major number」（這裡是 509），後面才能用它來建立裝置節點。

```bash
# 4. 用新的 major/minor（509/0）建節點，並設定權限
sudo mknod /dev/hello_char c 509 0
sudo chmod 666 /dev/hello_char
```

* **`sudo mknod /dev/hello_char c 509 0`**：

  * `mknod` 用來手動創建裝置檔案。
  * `/dev/hello_char`：要創建的路徑。
  * `c`：表示「字元設備」（character device）。
  * `509`：major number。
  * `0`：minor number（通常字元設備簡單只用 0）。
* **`sudo chmod 666 /dev/hello_char`**：將裝置檔權限設為 `rw-rw-rw-`，讓所有使用者都能讀寫。
* **為什麼要做**：只有創建了對應的裝置節點並賦予合適權限，使用者空間程式才能 `open("/dev/hello_char")` 並進行讀寫。

```bash
# 5. 測試
cat /dev/hello_char
```

* **作用**：用 `cat` 讀取 `/dev/hello_char`，觸發驅動的 `.read` 回呼函式（`hello_read`），把字元從核心複製到使用者緩衝區並輸出到終端。
* **預期結果**：終端上會看到

  ```
  Hello from PI!
  ```
* **為什麼要做**：最直接驗證驅動是否正常實作了 open → read → release 的流程，以及核心到使用者空間的資料傳遞是否正確。

---
```bash
# 第一次測試前
sudo insmod hello_char.ko
sudo mknod /dev/hello_char c <major> 0  2>/dev/null || true
sudo chmod 666 /dev/hello_char

# 讀取測試好幾次
cat /dev/hello_char
cat /dev/hello_char
…

# 如果改了程式要重載
sudo rmmod hello_char
sudo insmod hello_char.ko
# （不用再 mknod，只要節點還在）
cat /dev/hello_char

# 最後真的不需要再測試時
sudo rmmod hello_char
sudo rm /dev/hello_char

```
---

