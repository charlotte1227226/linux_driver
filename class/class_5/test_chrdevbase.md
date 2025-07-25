# 測試
前提:
```bash
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ gcc ./chrdevbaseAPP.c -o chrdevbaseAPP
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ file ./chrdevbaseAPP
./chrdevbaseAPP: ELF 64-bit LSB pie executable, ARM aarch64, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux-aarch64.so.1, BuildID[sha1]=ca1e1e861ce4c163b7f3ba64e092585e40ffdae2, for GNU/Linux 3.7.0, not stripped
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ uname -m
aarch64
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ 
```

1. 測試
    - `sudo ./chrdevbaseAPP /dev/chrdevbase`

# chrdevbase 虛擬設備區動的完善
要求: 應用程序可以對驅動讀寫操作，讀的話就是從驅動裡面讀取字符串，寫的話就是應用向驅動寫字符串。
1. `chrdevbase_read`驅動函數編寫

驅動給應用程序傳遞數據的時候需要用到 copy_to_user 函數。
Linux 核心提供的 `copy_to_user` 函數原型，完整內容是：

```c
static inline long copy_to_user(void __user *to,
                                const void *from,
                                unsigned long n);
```

* `to`：目標地址（使用者空間的緩衝區）
* `from`：來源地址（內核空間的緩衝區）
* `n`：要複製的位元組數

如果複製成功，回傳值為 **0**；如果複製失敗，回傳負值。
