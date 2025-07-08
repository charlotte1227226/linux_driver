# 尋找kernel路徑
## 取得當前核心版本
```bash
a0000@mcalab:~/Desktop/linux_driver/class $ uname -r
6.12.25+rpt-rpi-2712
```

## 確認編譯的kernel路徑
```bash
a0000@mcalab:~/Desktop/linux_driver/class $ ls -lh /lib/modules/6.12.25+rpt-rpi-2712/build
lrwxrwxrwx 1 root root 28 Jun 30 00:11 /lib/modules/6.12.25+rpt-rpi-2712/build -> /home/a0000/kernel-src/linux
```

## 確認實際載入的「壓縮核心映像」（vmlinuz/zImage）在哪裡
```bash
a0000@mcalab:~/Desktop/linux_driver/class $ ls -lh /boot/vmlinuz-6.12.25+rpt-rpi-2712
-rw-r--r-- 1 root root 9.3M Apr 30 20:45 /boot/vmlinuz-6.12.25+rpt-rpi-2712
a0000@mcalab:~/Desktop/linux_driver/class $ ls -lh /boot/initrd.img-6.12.25+rpt-rpi-2712
-rw-r--r-- 1 root root 13M Jun 26 22:30 /boot/initrd.img-6.12.25+rpt-rpi-2712
```

## 編譯候可能出現的問題
```bash
a0000@mcalab:~/Desktop/linux_driver/class/class_1 $ make
make -C /lib/modules/6.12.25+rpt-rpi-2712/build M=/home/a0000/Desktop/linux_driver/class/class_1 modules
make[1]: Entering directory '/home/a0000/kernel-src/linux'
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_1/chrdevbase.o
  MODPOST /home/a0000/Desktop/linux_driver/class/class_1/Module.symvers
ERROR: modpost: missing MODULE_LICENSE() in /home/a0000/Desktop/linux_driver/class/class_1/chrdevbase.o
make[3]: *** [scripts/Makefile.modpost:145: /home/a0000/Desktop/linux_driver/class/class_1/Module.symvers] Error 1
make[2]: *** [/home/a0000/kernel-src/linux/Makefile:1897: modpost] Error 2
make[1]: *** [Makefile:224: __sub-make] Error 2
make[1]: Leaving directory '/home/a0000/kernel-src/linux'
make: *** [Makefile:15: kernel_modules] Error 2
```

你在編譯時遇到的錯誤：

```
ERROR: modpost: missing MODULE_LICENSE() in …/chrdevbase.o
```

表示你的模組程式裡沒有宣告授權字串，modpost 無法生成 `Module.symvers`，因此編譯失敗。解法很簡單：在你的驅動程式原始檔（如 `chrdevbase.c`）最底下加入：
```c
// … 你原本的 module_init/module_exit 與其他程式碼 …

MODULE_LICENSE("GPL");                 // 一定要加，否則 modpost 會報錯
MODULE_AUTHOR("a0000");                // 可選，但建議填寫
MODULE_DESCRIPTION("Simple Char Device Driver");  // 可選

```
---
### `MODULE_LICENSE("GPL");` 就是承認並宣告：你的模組是以 GNU 通用公共授權發佈，遵守開放原始碼與 copyleft 的規範。
---

回到專案根目錄，執行：

```bash
make clean
make
```
這時 `modpost` 就能找到 `MODULE_LICENSE()`，順利產生 `Module.symvers` 並編譯出 `chrdevbase.ko`。

---

加入 `MODULE_LICENSE("GPL")` 之後，編譯流程就不會再因為「missing MODULE\_LICENSE()」而中斷，你的模組也能合法呼叫那些只有 GPL 模組才能使用的核心符號。

## 成功畫面
```bash
a0000@mcalab:~/Desktop/linux_driver/class/class_1 $ make
make -C /lib/modules/6.12.25+rpt-rpi-2712/build M=/home/a0000/Desktop/linux_driver/class/class_1 modules
make[1]: Entering directory '/home/a0000/kernel-src/linux'
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_1/chrdevbase.o
  MODPOST /home/a0000/Desktop/linux_driver/class/class_1/Module.symvers
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_1/chrdevbase.mod.o
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_1/.module-common.o
  LD [M]  /home/a0000/Desktop/linux_driver/class/class_1/chrdevbase.ko
make[1]: Leaving directory '/home/a0000/kernel-src/linux'
```


## 補充說明
---
`ls -lh` 其實是把兩個選項組合起來用，分別是：
- `ls`：列出目錄或檔案清單
- `-l`（long format）：以長列表形式顯示，會有檔案權限（rwx）、硬連結數、擁有者、群組、檔案大小、修改時間、檔名等欄位
- `-h`（human-readable）：將檔案大小用可讀性更高的單位顯示（如 K、M、G），而不是直接顯示 byte 數字


```
-rw-r--r-- 1 root root 9.3M  Apr 30 20:45  /boot/vmlinuz-6.12.25+rpt-rpi-2712
```

* `-rw-r--r--`：檔案權限（owner 可讀寫、group 與 others 只讀）。
* `1`：硬連結數。
* `root root`：擁有者與群組都是 root。
* `9.3M`：檔案大小大約 9.3 MB。
* `Apr 30 20:45`：上次修改時間。
* `/boot/vmlinuz-6.12.25+rpt-rpi-2712`：這是「壓縮後的核心映像」（Compressed Kernel Image），相當於 zImage，開機時由 bootloader 載入並解壓執行。

```
-rw-r--r-- 1 root root 13M Jun 26 22:30  /boot/initrd.img-6.12.25+rpt-rpi-2712
```

* 同樣格式顯示權限、大小（13 MB）、修改時間等。
* `initrd.img-...` 則是「初始 RAM 磁碟映像」（initial ramdisk），開機時用來載入核心模組或做檔案系統掛載前的準備。

---

**總結**

1. `/boot/vmlinuz-6.12.25+rpt-rpi-2712` = 壓縮過的 Linux 核心映像（zImage），約 9.3 MB。
2. `/boot/initrd.img-6.12.25+rpt-rpi-2712` = 初始 RAM 磁碟映像，用於核心啟動初期載入模組，約 13 MB。

---