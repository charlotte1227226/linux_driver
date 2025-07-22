# linux driver
## linux驅動開發：
1. Linux driver 不寫作業系統（限約）
2. 根據 Linux 的驅動框架進行開發，一定要搞懂框架，尤其是 Linux 下幾種 driver 的掌握
3. 驅動最終表現出來是 /dev/xxx 文件，打開/關閉、讀寫
4. `.dts` 是描述有外設的設備（編訊息，如 I2C）

---
### linux 驅動 3 大類：

1. 字串設備驅動，可以不定長度的字元傳遞資料（IIC, SPI）
   　　例子：順序的數據傳輸

2. 塊設備驅動，可以固定大小區塊傳遞 和 轉換資料
   　　例子：硬碟、USB 隨身碟，不需整塊讀而每次訪問固定大小的資料塊
   　　　　50B、flash、SSD

3. 網路設備驅動

　一個設備不一定只落單一類型，ex. USB WIFI（網路 + 字符）

---

## 字符設備驅動開發：
1. 應用程式和驅動的交互原理
    - 驅動其實是擷取外設，or 傳感器數據，控制外設，數據再交給應用程式
    - Linux 驅動的編譯需要編寫一個驅動，還是我們只編寫一個簡單的腳本來用結合 APP。單片基下驅動和應用都是放到一個文件裡面，也就是混雜再一起。linux下的驅動和應用是完全分開的。
 
### 用戶空間和內核空間(為了安全性)
![image](https://hackmd.io/_uploads/SkFoN6AVle.png)

- 應用程序想要訪問內核資源要怎麼做:
    - 系統調用（System Call）
    - 異常（Exception）/ 中斷（Interrupt）
    - 陷入（Trap）
應用程序不會直接調用系統調用，而是通過API函數來間接調用系統調用，比入: POSIX、API和C庫(C語言的庫)等。UNIX類操作系統中最常用的編程接口就是POSIX。
應用程序使用open函數打開一個設備文件。
![image](https://hackmd.io/_uploads/SkbzwpCExl.png)

每一個系統調用都有一個系統調用編號，如下圖:
![image](https://hackmd.io/_uploads/r1LI_6RVgg.png)
系統調用處於內核空間，應用程序無法直接訪問，因此需要"陷入"到內核，方法就是軟中斷。陷入內核以後還要指定系統調用編號。
![image](https://hackmd.io/_uploads/Hy71FaAEee.png)

---
#### 補充說明
| 名稱           | 主動 / 被動 | 說明                                    | 實際例子                                                         |
| ------------ | ------- | ------------------------------------- | ------------------------------------------------------------ |
| **系統調用**     | ✅ 使用者主動 | 應用程式透過呼叫函式，請求 Kernel 執行某些操作（I/O、記憶體等） | `open()`, `read()`, `write()`, `socket()`, `fork()` 等 C 標準函式 |
| **異常 / 中斷**  | ❗ 被動    | CPU 或硬體打斷目前流程，通知 kernel 處理事件          | - 除以零（異常）<br>- 鍵盤輸入、網路封包到達、定時器到（中斷）                          |
| **陷入（trap）** | ✅ 主動    | 是進入系統呼叫的底層機制，用來從使用者模式切換到核心模式          | - x86 上的 `int 0x80`<br>- x86\_64 上的 `syscall` 指令             |

---

### 字符設備驅動開發流程
1. linux裡面一切皆文件，驅動設備的表現就是一個 /dev/ 下的文件，如: /dev/led ，應用程序調用open函數打開一個設備，比如: led 。應用程序通過 write 函數向 /dev/led 寫數據，比如寫 1 表示打開，寫 0 表示關閉。如果要關閉設備那麼就是 close 函數。
2. 編寫驅動的時候，也需要對應的 open, close, write函數等等。字符設備驅動 struct file_operations 結構體。
3. 驅動最終是被應用程序調用的，在寫驅動的時候要考慮應用開發的便利性。
4. 驅動是分驅動框架的，要按照驅動框架來編寫，對於字符設備驅動來說，重點編寫應用程序對應的 open, close, read, write 等函數。

---
### 我的第一個linux驅動字符設備驅動
##### 字符設備驅動簡介
字符設備是 Linux 驅動中最基本的一類設備驅動，字符設備就是一個一個字節，按照字節流進行讀寫操作的設備，讀寫數據是先後順序的。例如我們最常見的點燈、按鍵、IIC、SPI、LCD 等等都是字元設備，這些設備的驅動就叫做字符設備驅動。
![image](https://hackmd.io/_uploads/S1cJUAC4ge.png)

#### 字符設備驅動框架
字符設備驅動的編寫主要就是對應的 open, close, read ...其實就是file_operations 結構體的成員變量的實現。

---
#### 補充說明
其中關於 C 函式庫以及如何透過系統呼叫陷入到核心空間這個我們不用去管，我們重點關注的是應用程式和具體的驅動，應用程式使用到的函式在具體驅動程式中都有與之對應的函式。例如應用程式中調用了 `open` 這個函式，那麼在驅動程式中也得有一個同名為 `open` 的函式。每一個系統呼叫，在驅動中都有與之對應的一個驅動函式，在 Linux 核心檔案 include/linux/fs.h 中有一個叫做 file_operations 的結構體，此結構體就是 Linux 核心驅動操作函式集合，內容如下所示：
![image](https://hackmd.io/_uploads/ryzs8AAExl.png)

---
#### 驅動模塊的加載與卸載
linux驅動程序可以編譯到 kernel裡面，也就是zImage，也可以編譯為模塊， .ko。測試的時候只要加載 .ko 模塊就可以了。

編寫驅動的時候的注意事項:
1. 編譯驅動的時候需要用到 linux 內核源碼! git clone 對應 linux kernel source code ，並且編譯(make)，得到 zImage 和 .dtb 。需要使用編譯後得到的 zImage 和 .dtb 。

2. 從SD卡啟動, SD卡燒寫了 uboot。 uboot 通過 tftp 從ubuntu裡面或去zImage和dtb，rootfs也是通過nfs掛載

3. 編譯出來的 .ko 文件放到跟文件系統裡面。用加載驅動會用到加載命令:`insmod`, `modprobe`。移出驅動使用`rmmod`。對於一個新的模塊使用`modprobe`加載的時候需要先調用一下`depmod`命令
4. 模塊加載成功可以用`lsmod`查看
---
#### 補充說明
#### 40.2.1 驅動模組的加載和卸載

Linux 驅動有兩種運行方式，第一種就是將驅動編譯進 Linux 內核中，這樣當 Linux 內核啟動的時候就會自動運行驅動程序。第二種就是將驅動編譯成模組（Linux 下模組副檔名為 .ko），在 Linux 內核啟動以後使用 `insmod` 命令加載驅動模組。在調試驅動的時候一般都選擇將其編譯為模組，這樣我們修改驅動以後只需要重新編譯一下驅動代碼即可，不需要編譯整個 Linux 代碼。而且在調試的時候只需要加載或卸載模組即可，不需要重啟整個系統。總之，將驅動編譯為模組最大的好處就是方便調試，當驅動開發完成、確定沒有問題以後就可以將驅動編譯進 Linux 內核中，當然也可以不編譯進 Linux 內核中，具體看自己的需求。

模組的加載和卸載兩種操作，我們在編寫驅動的時候需要註冊這兩個操作函數，模組的加載和卸載註冊函數如下：

```c
module_init(xxx_init);  // 註冊模組加載函數
module_exit(xxx_exit);  // 註冊模組卸載函數
```

`module_init` 函數用來向 Linux 內核註冊一個模組加載函數，參數 `xxx_init` 就是需要註冊的具體函數，當執行 `insmod` 命令加載模組的時候，`xxx_init` 這個函數就會被調用。`module_exit()` 函數用來向 Linux 內核註冊模組卸載函數，`xxx_exit` 就是需要註冊的具體函數，當使用 `rmmod` 命令卸載某個模組的時候 `xxx_exit` 函數就會被調用。
![image](https://hackmd.io/_uploads/BJUSpAAExx.png)

---

在樹莓派（以及大多數 ARM 平台）上，`zImage` 和各種 `.dtb` 檔案是開機過程中「引導程式（bootloader）」要讀取的兩個關鍵檔案：

1. **`zImage`（Compressed Kernel Image）**

   * 這是「壓縮後」的 Linux 核心映像檔（kernel image）。
   * 引導程式會先把它載入記憶體，然後解壓縮（inflate）成真正的執行態核心（vmlinux）。
   * 之所以用壓縮形式，一方面可以減少在儲存媒體／記憶體佔用，另一方面在早期 ARM 架構板子上也必須先用一小段解壓程式把核心載入。
   * 在 Raspberry Pi 上，這個 `zImage` 通常會放在 SD 卡的 `/boot` 分割區，並在 `config.txt` 用 `kernel=zImage` 指定。

2. **`.dtb`（Device Tree Blob）**

   * Device Tree 是一種用來描述硬體拓撲與配備的資料結構（例如：CPU 型號、記憶體大小、外設中斷號、GPIO 分腳對應、時脈來源…等）。
   * DTS（.dts）是「原始碼」格式，編譯後就變成二進位的 `.dtb`（Device Tree Blob）。
   * 核心啟動時，會搭配這份 `.dtb`，知道板子上到底有哪些外設／如何初始化它們，不需要把所有驅動都寫死在核心裡。
   * Raspberry Pi 標準做法是把多個型號對應的 `.dtb`（例如 `bcm2711-rpi-4-b.dtb`、`bcm2711-rpi-5-b.dtb`）放在 `/boot`，再在 `config.txt` 用 `device_tree=` 或透過 `dtoverlay=` 選擇要載入哪一個。

---

### 為什麼要自己編譯出 zImage 與 .dtb？

* **版本對應性**：你下載的樹莓派官方韌體附帶的核心（zImage）和 DTB 版本，可能和你自己從 GitHub 抓的 kernel tree 分支（例如 `rpi-6.12.y`）不一樣。為了保證 out-of-tree 模組 (hello\_char.ko) 能和核心版本、符號表一一對應，你需要用相同的原始碼編出 zImage、modules\_prepare 時生成的符號表，還要配上對應的 .dtb。
* **測試自訂選項**：如果你在 kernel config（`.config`） 裡打開或關閉了某些選項、或更新了驅動程式才能正確測試，也要自己編 kernel。

---

#### 總結

* **zImage**＝「壓縮後的核心執行檔」，引導程式載入並解壓縮後才是真正跑起來的 Linux 核心。
* **.dtb**＝「硬體描述二進位檔」，告訴核心它跑在哪塊板子上，要怎麼初始化記憶體、外設、時脈與中斷等。

了解這兩個檔案的角色之後，你就能把它們一起放到 SD 卡 `/boot`，配合你的驅動模組進行完整測試了。

---
### 字符設備註冊與註銷
1. 我們需要像系統註冊一個字府設備的時候，使用register_chrdev函數
2. 卸載驅動的時候需要註銷掉前面的字符設備，使用unregister_chrdev函數

**設備號（`dev_t`）簡要說明**

1. **型別定義**

   * 在 `include/linux/types.h` 中：

     ```c
     typedef __u32   __kernel_dev_t;
     typedef __kernel_dev_t   dev_t;
     ```
   * 而 `__u32` 在 `include/uapi/asm-generic/int-ll64.h` 中定義為：

     ```c
     typedef unsigned int   __u32;
     ```

   ⇒ 因此 `dev_t` 本質上就是 32 位元的 `unsigned int`（無符號整數）。

2. **位元配置**
   Linux 內核把 `dev_t` 的 32 位切成兩段：

   * **主設備號（major）**：佔高 12 位，範圍 0～4095。
   * **次設備號（minor）**：佔低 20 位，範圍 0～(2²⁰−1)。

3. **操作巨集（Macro）**

   ```c
   #define MINORBITS   20
   #define MINORMASK   ((1U << MINORBITS) - 1)

   #define MAJOR(dev)  ((unsigned int)((dev) >> MINORBITS))
   #define MINOR(dev)  ((unsigned int)((dev) & MINORMASK))
   #define MKDEV(ma,mi)  (((ma) << MINORBITS) | (mi))
   ```

   * `MINORBITS`：次設備號佔用的位元數（20 位）。
   * `MINORMASK`：用來擷取低 20 位的掩碼。
   * `MAJOR(dev)`：將 `dev` 右移 20 位，取出主設備號。
   * `MINOR(dev)`：以掩碼 AND `dev`，取出次設備號。
   * `MKDEV(ma,mi)`：將主號左移 20 位，再 OR 上次號，組合成一個 `dev_t`。

4. **使用情境**

   * 取得主/次號：

     ```c
     unsigned int ma = MAJOR(dev);
     unsigned int mi = MINOR(dev);
     ```
   * 合成設備號：

     ```c
     dev_t dev = MKDEV(ma, mi);
     ```

   通常搭配 `register_chrdev(ma, name, fops)`、`cdev_add()` 等函式來註冊字元設備。
