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

## copy_to_user
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

---

```c
void __user *to;
```
不是「指向 `__user` 的指標」，而是「帶有 `__user` 標記的 `void*`」。
等同於：

* 類型上它是一個 `void *`（不帶型別的指標），
* 前面加了 `__user` 這個標記（annotation）告訴編譯器／靜態分析工具：

  > 這個指標所指向的記憶體位於「使用者空間」，不能在內核中直接 `*to` 存取，必須透過 `copy_to_user`、`get_user`、`put_user` 等介面才能安全操作。

所以 `__user` 不是一個獨立的類型，而是附加在指標上的屬性，用來區分「內核空間指標」和「使用者空間指標」。

---
## copy_from_user
`copy_from_user` 的原型和用法跟 `copy_to_user` 很像，只是方向相反──把資料從「使用者空間」拷貝到「核心空間」。在大多數 Linux 版本中，它的原型定義大致如下：

```c
static inline long copy_from_user(void *to,
                                  const void __user *from,
                                  unsigned long n);
```

* `to`   ：目標地址，位於**核心空間**的緩衝區
* `from` ：來源地址，加了 `__user` 標記，位於**使用者空間**
* `n`    ：要複製的位元組數

**回傳值**

* 回傳 **0** 表示全部複製成功
* 若有失敗，回傳的是「沒複製到」的位元組數（>0）

---