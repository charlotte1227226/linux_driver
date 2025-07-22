# 應用程序的編寫
linux下一切都是文件，首先要 `open`，(透過`man 2 open`裡查看要調用什麼函示庫，在樹梅派上`open`要用`#include <fcntl.h>`)

# chrdevbase 虛擬設備驅動對的編寫


---
## 終端機結果
```bash
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ gcc ./chrdevbaseAPP.c -o chrdevbaseAPP
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ file ./chrdevbaseAPP
./chrdevbaseAPP: ELF 64-bit LSB pie executable, ARM aarch64, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux-aarch64.so.1, BuildID[sha1]=ca1e1e861ce4c163b7f3ba64e092585e40ffdae2, for GNU/Linux 3.7.0, not stripped
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ uname -m
aarch64
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ 
```

## 補充說明
Linux 的 `man`（manual，手冊）系統是如何把內容分門別類的，以及如何指定想看的章節（section）：

1. **man 手冊的章節（section）分類**
   Linux 常見把 `man` 手冊分成以下幾個 section：

   ```
   1  標準指令（Standard commands）
   2  系統呼叫（System calls）
   3  程式庫函式（Library functions）
   4  特殊裝置（Special devices）
   5  檔案格式（File formats）
   6  遊戲和娛樂（Games and toys）
   7  其他雜項（Miscellaneous）
   8  管理員指令（Administrative commands）
   9  Linux 核心例行程式（Linux-specific kernel routines）
   ```

2. **為什麼要指定 section？**
   有些名稱一樣的條目，可能同時在多個 section 出現──例如 `open` 既有一般指令（section 1），也有系統呼叫（section 2）；如果你只下 `man open`，系統預設會先顯示 section 1 的說明，也就是命令介面的 `open`。

   如果你想看 `open(2)`（系統呼叫版）的說明，就要指定章節：

   ```bash
   man 2 open
   ```

3. **怎麼用？**
   在 `man` 後面加數字，就是指定那個 section：

   ```bash
   man 1 ls       # 查看 ls 這個指令（section 1）
   man 3 printf   # 查看 printf 這個 C 函式（section 3）
   man 2 open     # 查看 open 這個系統呼叫（section 2）
   ```

這樣就能精確地跳到你想要的手冊內容了。

```bash
NAME
       open, openat, creat - open and possibly create a file

LIBRARY
       Standard C library (libc, -lc)

SYNOPSIS
       #include <fcntl.h>

       int open(const char *pathname, int flags);
       int open(const char *pathname, int flags, mode_t mode);
```

其中要寫open的話要用`#include <fcntl.h>`