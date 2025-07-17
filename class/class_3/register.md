# 在 Linux 核心裡如何註冊（register）和銷毀（unregister）一個字元設備。重點如下：

## 1. 函數原型

```c
// 註冊字元設備
static inline int register_chrdev(unsigned int major,
                                   const char *name,
                                   const struct file_operations *fops);

// 取消註冊字元設備
static inline void unregister_chrdev(unsigned int major,
                                     const char *name);
```

---

## 2. 參數說明

1. **major**

   * 主要裝置號（major number），Linux 用它來區分不同的驅動類型。
   * 每個字元或區塊設備都要分配一個主、次（minor）號，major 就是主號。

2. **name**

   * 一個以 `\0` 結尾的字串，用來命名這個設備（例如 `"chrdevbase"`），也會在 `/proc/devices` 中顯示。

3. **fops**

   * 指向 `struct file_operations` 的指標，裡面包含了 `open`、`read`、`write`、`ioctl`… 等操作函數的集合。

---

## 3. 何時呼叫

* **module 入口函數** (`xxx_init`) 中呼叫 `register_chrdev`，完成字元設備的註冊。
* **module 退出函數** (`xxx_exit`) 中呼叫 `unregister_chrdev`，卸載時把設備從系統中移除。

---

## 4. 範例流程

```c
static struct file_operations test_fops = {
    // .open    = test_open,
    // .read    = test_read,
    // .write   = test_write,
    // ……
};

static int __init xxx_init(void)
{
    int ret = register_chrdev(240, "mychardev", &test_fops);
    if (ret < 0) {
        printk(KERN_ERR "register_chrdev failed\n");
        return ret;
    }
    printk(KERN_INFO "mychardev registered, major=240\n");
    return 0;
}

static void __exit xxx_exit(void)
{
    unregister_chrdev(240, "mychardev");
    printk(KERN_INFO "mychardev unregistered\n");
}

module_init(xxx_init);
module_exit(xxx_exit);
MODULE_LICENSE("GPL");
```

1. **註冊**：叫作 `register_chrdev(major, name, &fops)`
2. **卸載**：叫作 `unregister_chrdev(major, name)`

---

**總結**：

* `register_chrdev` → 分配 major 號並綁定操作函數，讓使用者空間能開 `/dev/…` 連到你的 driver。
* `unregister_chrdev` → 卸下這個字元設備，清理系統資源。

---
`static inline` 在 Linux 核心（也在一般 C 語言）裡，其實是把兩個關鍵字疊在一起使用，分別代表：

---

## 1. `inline`

* **提示編譯器**：把這支函式「內嵌」到呼叫它的地方，換句話說，編譯器會嘗試 **展開** 函式本體，取代一次函式呼叫與返回的成本。
* **優化呼叫效率**：對於很短小、執行效能敏感的小函式（像是在頭檔裡包裝一些簡單邏輯），`inline` 可以省掉 call/return 的開銷。
* **並非強制**：它只是「建議」，編譯器還是會根據優化等級與函式體大小，決定要不要真的展開。

## 2. `static`

* **限制可見性**：把函式的「連結性」變成「內部」(internal linkage)，也就是這個函式只對**當前翻譯單元**（那個 .c 檔或包含它的 .c 檔）可見。
* **避免重複定義**：如果你把這段 `static inline` 函式寫在一個 header（.h）裡，並被多個 .c 同時包含，每個 .c 都會得到自己的一份「本地版本」，不會和別的 .c 衝突。

---

### 為什麼 Linux 核心常用它？

1. **放在頭檔** (`.h`) 裡，供多個模組使用時不會連結失敗。
2. **小且頻繁呼叫** 的包裝函式（wrapper），內嵌後效能較好。
3. **清晰又安全**：用 `static` 隔離連結範圍，用 `inline` 優化呼叫成本。

---

### 範例

```c
// include/linux/foo.h
static inline int foo_max(int a, int b)
{
    return (a > b) ? a : b;
}
```

* **任何** `#include "foo.h"` 的 .c 檔，都可以呼叫 `foo_max(a,b)`，
* 編譯器很可能會把它展開成 `(a > b) ? a : b`，
* 且不會因為多個 .c 同時包含而出現「重複定義」的連結錯誤。

---

**總結**：

* `inline` → 嘗試把函式「展開」進呼叫點，提升效能。
* `static` → 把函式限制在本翻譯單元，避免多重定義衝突。
* **合併使用** → 很適合寫在頭檔、提供小型工具函式，又能放心被多處包含、同時享用內嵌優化。

---
「展開函式本體」指的是：
當編譯器遇到對這支被標記為 `inline` 的函式的呼叫時，它不是真的跳去呼叫那支函式再回來，而是**直接把那支函式裡的程式碼**，**貼進呼叫的地方**，就像你把函式內容手動複製到那裡一樣。

---

### 例子對比

#### 一般呼叫

```c
int add(int a, int b) {
    return a + b;
}

int foo() {
    return add(2, 3);  // runtime 會跳到 add() 再跳回來
}
```

#### inline 展開

```c
static inline int add(int a, int b) {
    return a + b;
}

int foo() {
    // 編譯器可能把 add 展開成：
    return 2 + 3;   // 直接用運算式，沒有函式呼叫開銷
}
```

* **優點**：省去「呼叫→跳轉→返回」的額外指令，執行速度更快。
* **缺點**：如果函式太大、呼叫很多次，展開後程式碼膨脹，可能會增大二進位檔體積。

簡單來說，「展開函式本體」就是把函式「內文貼到呼叫處」的優化手法。
