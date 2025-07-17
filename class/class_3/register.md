# 註冊流程
```bash
cat /proc/devices
```

所看到的「字元裝置（Character devices）」列表，意義如下：

* **左欄**：主設備號（Major Number）── Linux 核心用它來區分不同的裝置驅動。
* **右欄**：裝置名稱（Driver Name）── 對應 `/dev` 底下那些字元設備節點所屬的驅動。

舉例說明：

```
 1  mem       ← Major 1 是「mem」（/dev/mem, /dev/kmem 等）  
 4  tty       ← Major 4 是「tty」（傳統終端）  
 5  ptmx      ← Major 5 是「ptmx」（pseudo‑tty 多工器）  
```

這個檔案還會接著列出「區塊裝置（Block devices）」的 Major Number 對照表，幫你快速查到 kernel 中哪些 major 已被註冊、對應到什麼驅動。

---
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


---
## Linux 核心裡用來表示「設備號」的 `dev_t` 到底是什麼型別──答案是一個 **32 位元的無符號整數**。它是透過一連串的 `typedef` 定義出來的：

1. 在 **`include/linux/types.h`** 裡：

   ```c
   typedef __u32   __kernel_dev_t;
   typedef __kernel_dev_t  dev_t;
   ```

   * 先將 `__u32` 重新命名為內核用的 `__kernel_dev_t`，再把它別名為 `dev_t`。

2. 在 **`include/uapi/asm-generic/int-ll64.h`**（或其他架構相關的標頭檔）裡：

   ```c
   typedef unsigned int  __u32;
   ```

   * 因此 `__u32` 就等同於 `unsigned int`，也就是 32 位元的無符號整數。

---

**綜上所述**，`dev_t` 本質上就是 `unsigned int` 型別，佔用 32 位元，用來在核心中封裝並存放主設備號（major）與次設備號（minor）。

---
## 講Linux 核心裡怎麼把一個 `dev_t`（設備號）打包成「主設備號＋次設備號」，以及怎麼再拆出來。重點是：

1. **位元配置**

   * `MINORBITS` 定義了「次設備號」佔用的位元數：20 位。
   * 由此「主設備號」就佔了剩下的 32−20＝12 位，所以範圍是 0～4095。

2. **掩碼**

   ```c
   #define MINORMASK   ((1U << MINORBITS) - 1)
   ```

   把低 20 位都設成 1，用來「擷取」次設備號。

3. **拆出主／次號**

   ```c
   #define MAJOR(dev)  ((unsigned int)((dev) >> MINORBITS))
   #define MINOR(dev)  ((unsigned int)((dev) & MINORMASK))
   ```

   * `MAJOR(dev)` → 把 `dev_t` 右移 20 位，就只剩高 12 位的「主設備號」。
   * `MINOR(dev)` → 用掩碼把低 20 位「切出來」，就是「次設備號」。

4. **合成一個 `dev_t`**

   ```c
   #define MKDEV(ma,mi)  (((ma) << MINORBITS) | (mi))
   ```

   * 先把主號左移 20 位，空出低 20 位；
   * 再跟次號做位元 OR，就得到完整的 32 位設備號。

---

**簡單比喻**：

* 把 `dev_t` 想像成一個 32 位的容器，

  * 最高 12 位放「主號」，
  * 低 20 位放「次號」。
* `MKDEV` 就是在組裝這個容器，
* `MAJOR`／`MINOR` 則是拆開來拿各自的數值。

---
# 設備號（`dev_t`）簡要說明

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

---
## 宏函數
在 C 語言裡，「宏函數」（Function‑like Macro）其實不是「真正的函式」，而是由 **預處理器**（preprocessor）在編譯前做 **文字取代**（textual substitution）的一種機制。它的特點是：

---

## 1. 定義方式

使用 `#define` 搭配參數，就像這樣：

```c
#define SQUARE(x)  ((x) * (x))
```

* `SQUARE` 就是一個「宏函式」，接受一個參數 `x`。
* 它並不會產生一段可被呼叫的程式碼（像真正的函式那樣），而是在預處理階段，看到 `SQUARE(a+1)` 就直接把它「展開」成 `((a+1) * (a+1))`。

---

## 2. 特點比較

| 特性    | 宏函式 (`#define`)   | 內嵌函式 (`inline`)         |
| ----- | ----------------- | ----------------------- |
| 實作階段  | 編譯前（文字取代）         | 編譯時（函式呼叫優化）             |
| 型別檢查  | 不做型別檢查，容易出現副作用    | 有完整的型別檢查，安全性高           |
| 呼叫開銷  | 幾乎沒有（純粹展開運算式）     | 有可能保留呼叫概念（但編譯器會嘗試內嵌）    |
| 副作用風險 | 參數若有運算、或缺少小括號，易出錯 | 不會有文字替換問題               |
| 語法限制  | 只能做簡單的文字替換        | 能寫複雜邏輯、迴圈、條件…等標準 C 函式內容 |

---

## 3. 為什麼 Linux 用宏函式？

像剛才看到的：

```c
#define MAJOR(dev)   ((unsigned int)((dev) >> MINORBITS))
#define MINOR(dev)   ((unsigned int)((dev) & MINORMASK))
#define MKDEV(ma,mi) (((ma) << MINORBITS) | (mi))
```

* 這些宏函式會在預處理階段 **直接展開**，速度快、沒有呼叫開銷。
* 定義在標頭檔裡，任何包含它的 `.c` 檔都能使用，並且不會產生重複符號的連結錯誤。

---

### 小結

* **宏函式**：利用 `#define 名稱(參數)…`，靠 **文字替換** 做運算，沒有真正的呼叫開銷，但需注意括號與副作用。
* **函式**：真正由編譯器產生呼叫碼，有型別安全與調試便利，但若要最佳化呼叫開銷可加 `inline`。

---
**宏函數 (Function‑like Macro)** vs. **inline 函式** 的差別：

| 特性         | 宏函數 (`#define`)                         | inline 函式 (`static inline` 或 `inline`)                  |
| ---------- | --------------------------------------- | ------------------------------------------------------- |
| **實作時機**   | 編譯前由 **預處理器** 進行「文字置換」                  | 編譯階段由 **編譯器** 處理，可選擇真正內嵌或產生函式呼叫                         |
| **型別檢查**   | 無型別檢查，純粹文字替換，易出現參數傳遞錯誤                  | 有完整型別檢查，傳入庫型務必吻合函式參數型別                                  |
| **副作用**    | 參數若帶表達式或有自增自減，可能被多次評估 (副作用)             | 跟一般函式一樣，參數只評估一次                                         |
| **調試／診斷**  | 因為沒有真實呼叫堆疊，不易設定斷點或查看呼叫狀態                | 真正函式或內嵌後仍可在編譯器產生的符號中找到，調試友善                             |
| **程式碼膨脹**  | 展開後完全貼入呼叫處，若呼叫多次容易造成二進位膨脹               | 編譯器可依大小與優化策略選擇是否內嵌，較能控制程式碼尺寸                            |
| **定義位置**   | 必須用 `#define` 放在 header (.h)，但易與變數或函式衝突 | 普通函式語法，可放在 .c 或 .h，若加 `static inline` 放在 header 也不會重複定義 |
| **可讀性／維護** | 較生硬，易因括號或參數次序錯誤導致難察覺的 bug               | 跟一般函式一樣，易於閱讀、重構和重複使用                                    |

---

### 小結

* **宏函式** 適合做非常簡單、性能極端敏感的「文字替換」計算，但要非常小心加上足夠括號，並避免參數副作用。
* **inline 函式** 則兼具「呼叫效率」與「型別安全、可調試性」，更貼近一般函式風格，建議在核心或應用程式中優先使用。

---