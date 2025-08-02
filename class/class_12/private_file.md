# 文件私有數據
1. 在`open`函數裡面設置`filp->private_data`為設備變量。
    - `filp->private_data = &設備結構體;`
2. 在`read`、`write`裡面要訪問設備的時候，直接讀取私有數據
    - `struct 設備結構體 *dev = filp->private_data;`



# `goto`
## 當 driver probe／init 過程中某一步失敗，就用 `goto` 跳到對應的標籤，把先前已經分配的資源反向釋放，最後再回傳錯誤碼。還有避免重複代碼。
```c
if (ret < 0){
    printk("error");
    return -1;
}
```

---
在 Linux 字元裝置驅動裡，`open()`／`release()` 這兩個 callback 都會收到兩個「核心」結構體指標——`struct inode *inode` 與 `struct file *filp`。下面分別說明這兩者，以及 `filp->private_data`（也就是你存放 `&newchrled` 的那塊記憶體）內部通常會有哪些欄位。

---

## 1. `struct inode *inode`

這是 VFS（虛擬檔案系統）用來描述「檔案」或「裝置檔」的 metadata。你在 driver 裡看到的只是指標，但它底下包含了：

| 欄位            | 意義                                             |
| ------------- | ---------------------------------------------- |
| `i_mode`      | 檔案類型（普通檔、目錄、字元裝置、區塊裝置……）以及存取模式位元（rwx）          |
| `i_rdev`      | 如果是字元或區塊裝置，這裡記錄了 `dev_t`（major/minor）號         |
| `i_ino`       | Inode 編號（唯一識別同一個檔案）                            |
| `i_sb`        | 指向此 inode 所屬的 super\_block（檔案系統實例）             |
| `i_uid/i_gid` | 檔案擁有者的使用者／群組 ID                                |
| `i_op`        | 指向 `struct inode_operations`，定義對 inode 的各種操作函式 |
| ……            | 還有存取時間、修改時間、連結數等其他欄位                           |

**重點**：在字元裝置 driver 的 open 裡，你大多用不到其他欄位，只要知道 `inode->i_rdev` 會告訴你這個 open 是哪個 major/minor 裝置號就足夠了。

---

## 2. `struct file *filp`

這是 VFS 打開檔案後回傳給使用者層的「檔案描述符狀態」。它記錄了這把「檔案」打開時的 mode、flags、目前的讀寫指標等等。關鍵欄位例如：

| 欄位             | 意義                                                             |
| -------------- | -------------------------------------------------------------- |
| `f_flags`      | open 時的 flags，比如 `O_RDONLY`、`O_NONBLOCK`……                     |
| `f_mode`       | 讀寫模式，例如 `FMODE_READ` / `FMODE_WRITE`                           |
| `f_pos`        | 檔案讀寫游標（offset）                                                 |
| `f_inode`      | 指回剛才那個 `struct inode *`                                        |
| `f_op`         | 指向 `struct file_operations`，定義了後續的 `.read`/`.write`/`.ioctl` 等 |
| `private_data` | 驅動作者專用指標，你可以放任意東西，後續所有對這把「檔案」的呼叫都能拿回來                          |
| `f_path`       | 包含 `dentry` + `vfsmount`，代表檔案在檔系統樹中的位置                         |
| ……             | 還有 refcount、epoll/async 通知機制的欄位等等                              |

---

## 3. `filp->private_data` 的內容：`struct newchrled_dev`

這塊記憶體是你自己在 driver 裡定義的，用來儲存這個字元裝置的所有「狀態」或「資源」。舉例來說，一個常見的 LED driver 可能長這樣：

```c
struct newchrled_dev {
    int                major;       // 註冊到 kernel 的主裝置號
    struct cdev        cdev;        // 核心的字元裝置抽象
    void __iomem      *gpio_base;   // 我用 ioremap() 映射的 GPIO 控制暫存器位址
    struct class      *cls;         // sysfs 上的 class
    struct device     *dev;         // sysfs 上的 device
    spinlock_t         lock;        // 可能的同步鎖
    bool               led_on;      // LED 目前是亮還是不亮
    /* …其他你需要的參數… */
};
```

* **`major`**：你在 `register_chrdev_region()` 或 `alloc_chrdev_region()` 拿到的主裝置號
* **`cdev`**：用 `cdev_init()` / `cdev_add()` 把你的 `file_operations` 連到這個裝置上
* **`gpio_base`**：`ioremap()` 後可以直接用 `readl()`/`writel()` 控制硬體
* **`cls` / `dev`**：配合 `class_create()`、`device_create()` 讓 `/sys/class/...`、`/dev/newchrled` 都能正確顯示
* **`lock`**：若在多執行緒或中斷上下文要保護狀態就會用到
* **`led_on`**：紀錄目前狀態，讓你的 `write()` 讀到 user-space 指令後能正確切換

---

### 小結

1. `inode`：檔案系統層級的 metadata，主要讓核心知道是哪個檔案／哪個裝置號。
2. `filp`：每次 `open()` 都會產生一個「檔案描述符實例」，記錄你的 open flags、游標、以及 pointer to driver context（`private_data`）。
3. `filp->private_data`：完全由驅動自己定義，用來把裝置的整個結構體（像是 GPIO 位址、cdev、LED 狀態⋯⋯）綁在這次 open 上，後續的 read/write/ioctl 都能很方便地拿到同一組資源。

只要掌握這三者的角色，就能輕鬆在各個 callback 間傳遞並管理你的裝置狀態啦！

---
在 Linux 驅動裡，這兩個函式分別是對應到使用者空間對你的角色裝置檔案 (`/dev/newchrled` 之類) 呼叫 `open()` 與 `close()` 時所會執行的 callback，也就是 `file_operations` 結構裡的 `.open` 和 `.release`：

1. **`newchrled_open`**

   ```c
   static int newchrled_open(struct inode *inode, struct file *filp)
   {
       filp->private_data = &newchrled;
       return 0;
   }
   ```

   * 當使用者程式呼叫 `open("/dev/newchrled", …)` 時，kernel 會進入這個函式。
   * `inode` 代表檔案系統的 inode 資訊；`filp`（`struct file *`）代表這次開啟的檔案描述符。
   * `filp->private_data` 是一個 void\*，驅動作者可以把它當成「這次 open 會話」要用到的上下文 (context) 指標。
   * 這裡把全域的 `newchrled` 裝置結構體指標存進去，後續在其他 `.read`、`.write` 或 `.ioctl` 等函式就能透過 `filp->private_data` 拿到同一個結構。

2. **`newchrled_release`**

   ```c
   static int newchrled_release(struct inode *inode, struct file *filp)
   {
       struct newchrled_dev *dev = (struct newchrled_dev *)filp->private_data;
       return 0;
   }
   ```

   * 當使用者程式呼叫 `close(fd)` 或程式結束自動 close 時，就會執行這裡。
   * 這裡再把 `filp->private_data` 轉回原本的 `struct newchrled_dev *`，如果有需要做清理（像是釋放記憶體、關閉硬體資源）就可以在這裡做。
   * 目前實作裡沒做任何清理，所以只是「取出卻不動作」，然後回傳 0。

---

**重點總結**

* 它們都是驅動裡的 open/close hooks，分別在裝置檔案被 open/close 時跑。
* `filp->private_data` 用來在同一次 open-session 中，讓驅動的其他操作函式共享同一份裝置結構體或狀態。
* `newchrled_open` 負責初始化這個指標；`newchrled_release` 則是把它拿回去（以便未來若要釋放或清理，可以在這裡做）。

---

在 C/C++ 程式中，記憶體通常被連結器（linker）和啟動程式（startup code）分成好幾個區段（segment），最常見的是：

1. **`.text` 段**

   * 存放程式的機器碼（程式指令），會被載入到執行記憶體（通常是 Flash 或 ROM）中。
2. **`.data` 段**

   * 存放已初始化的全域（global）或靜態（static）變數。
   * 例如：`int a = 5;` 這種有明確初值的變數，它的初值（5）會一併儲存在執行映像裡，程式一載入就把它拷貝到 RAM。
3. **`.bss` 段**

   * 存放「未初始化」的全域或靜態變數。
   * 例如：`int counter;` 或 `static char buf[128];`，因為沒有給初值，連結器就把它們放在 bss 區。
   * **重點：bss 段本身在執行映像裡不佔空間存放初值，啟動程式會將這整塊區段一次性清為零。**

---
# 全域變量，未初始化會在bss區段，bss會自動清成零
### bss 段自動清零的意義

1. **保證預設值為 0**
   所有放在 bss 的變數在程式一開始執行前，都會自動被寫成 0（或對指標來說是 NULL，對浮點數是 0.0…）。

2. **節省映像大小**

   * 如果把未初始化變數當成已初始化（初值 = 0）放到 `.data`，就必須在映像裡儲存一大塊全是 0 的資料。
   * 將它們放在 bss，只要在 linker script 標記一段大小，實際檔案不需要存這些 0，透過啟動程式快速清除即可，減少使用的 Flash/ROM 空間。

3. **由誰來清？**

   * 在大多數裸機（bare‐metal）或 RTOS 環境下，系統啟動時有一段「C runtime startup」程式（常見叫做 `crt0` 或 `Reset_Handler`），它會：

     1. 將 `.data` 段從 Flash 複製到 RAM
     2. 將 `.bss` 段整塊填 0
     3. 最後呼叫 `main()`
   * 因此，程式一進到 `main()`（或使用者的初始化程式）時，所有 bss 區的變數都已經是零。

---

### 舉例

```c
// 放在 .data（已初始化）
int x = 42;     

// 放在 .bss（未初始化，會被清零）
int counter;    
static char buf[128];  

int main(void) {
    // 這時候：
    //   x       == 42
    //   counter == 0
    //   buf[...] 全部 == 0
    ...
}
```

---

### 為什麼要這樣設計？

* **效能與空間兼顧**：在嵌入式系統中，Flash/ROM 容量往往有限，把大量默認為 0 的變數留到執行時再一併清除，能省下映像檔大小，也能加快上電後的初始化速度（一次記憶體填寫 vs. 逐個變數設定）。
* **語意清晰**：C 標準保證「未初始化的全域／靜態變數預設為零」，bss 段清零正好落實了這條規範。

---

**總結**：「全局变量，未初始化会在 bss 段，bss 段会自动清零」的意思是：

> 編譯出來的程式，所有沒賦初值的全域或 `static` 變數，都被放在記憶體的 bss 區段，啟動程式會在 `main()` 執行前，將這整段記憶體一次性填滿 0，保證它們一開始都是零值。

# `goto`
這段程式碼其實是 Linux 驅動常見的「錯誤回復／清理（error-handling／cleanup）」機制：當 driver probe／init 過程中某一步失敗，就用 `goto` 跳到對應的標籤，把先前已經分配的資源反向釋放，最後再回傳錯誤碼。具體到你貼出的那幾行：

```c
fail_pdev:
    cdev_del(&srom_cdev);                        // 刪除之前用 cdev_add() 加入的 cdev 物件
fail_chrdev:
    unregister_chrdev_region(dev, srom_devs);    // 釋放向 kernel 註冊的 major/minor 編號
fail_mem:
    kfree(srom_devices);                         // 釋放前面 kmalloc() 的記憶體
    return result;                               // 回傳錯誤
```

1. **`cdev_del(&srom_cdev);`**
   把之前呼叫過的 `cdev_init()` + `cdev_add()` 所註冊到 VFS 的字元裝置結構體拿掉。

2. **`unregister_chrdev_region(dev, srom_devs);`**
   釋放 `alloc_chrdev_region(&dev, 0, srom_devs, "srom")` 或 `register_chrdev_region()` 當初分配給你的 major/minor 範圍，讓這些號碼回到系統可用池。

3. **`kfree(srom_devices);`**
   把前面用 `kmalloc()` 配置的一連串裝置結構體記憶體釋放掉。

整個流程就是：

* 初始化成功才一步步往下；
* 若中途某步失敗，就 `goto fail_Xxx`，從那個標籤開始「反向」清理所有資源，避免記憶體洩漏或裝置號碼被鎖死。

這種寫法可確保 driver 在任何失敗點都能乾淨地把已分配的資源回收。
