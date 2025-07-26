# LED 點燈驅動實驗(直接操作暫存器)

## 地址映射
1. 裸機LED燈實驗就是操作暫存器。
2. linux驅動開發也可以做暫存器，linux 不能直接對暫存器的物理地址進行讀寫操作，比如暫存器A物理地址為0X01010101。裸機的時候可以直接對0X01010101這個物理地址進行操作，但是linux不行。因為linux會使能MMU

在linux裡面操作的都是虛擬地址，所以需要先得到0X01010101這個物理地址對應的虛擬地址。
- 獲得物理地址對應的虛擬地址使用ioremap函數。
    - 第一個參數是物理地址其大小
    - 第二個參數就是要轉化的字節數量。
    - 0X01010101開始10個地址進行轉換，
    - `va = ioremap(0X01010101, 10);`
- 卸載驅動時要釋放掉ioremap的虛擬地址。
    - `iounmap(va);`

---
> **在撰寫驅動程式之前，要先簡單認識 MMU（Memory Management Unit，記憶體管理單元）**
>
> * 在舊版 Linux 裡，內核強制要求處理器必須具有 MMU；但現在的 Linux 已經支援沒有 MMU 的處理器了。
> * **MMU 的主要功能：**
>
>   1. **虛擬—物理位址映射**（Virtual ↔ Physical Address Translation）
>   2. **記憶體保護**：設定存取權限、快取屬性等
> 本文重點放在**第 1 點**——「虛擬空間到物理空間的映射」，也稱作地址映射。
>
> * **虛擬地址（VA，Virtual Address）**：程式執行時看到的位址範圍；對 32 位元 CPU 而言，就是 2³² = 4 GB。
> * **物理地址（PA，Physical Address）**：實際在硬體上對應的那塊記憶體；舉例開發板上只有 512 MB DDR3 物理內存。
> * MMU 的工作就是把這 512 MB 的物理內存「映射」到整個 4 GB 的虛擬空間，程式就好像在操作一整片 4 GB 的記憶體一樣；實際的分佈關係如書中圖 41.1.1 所示。

---
這段文字在講 `ioremap` 這個介面在 ARM 平台上的實作細節，大意如下：

1. **定義在哪裡**
   在 `arch/arm/include/asm/io.h` 裡，你會看到：

   ```c
   #define ioremap(cookie, size) \
       __arm_ioremap((cookie), (size), MT_DEVICE)
   ```

2. **真正執行的函式**
   `ioremap` 其實只是個巨集，它把兩個參數 `cookie`（實際上就是要映射的物理位址）和 `size`（映射長度）硬套上第三個參數 `MT_DEVICE`，再呼叫：

   ```c
   void __iomem * __arm_ioremap(phys_addr_t phys_addr,
                                size_t size,
                                unsigned int mtype)
   {
       return arch_ioremap_caller(phys_addr, size, mtype,
                                  __builtin_return_address(0));
   }
   ```

   * `arch_ioremap_caller` 是各架構（ARM）實際把物理頁映射到虛擬空間的底層函式。
   * `__builtin_return_address(0)` 則是把呼叫 `ioremap` 的程式位置也傳給它，用於除錯或追蹤（Symbolic Debugging）。

3. **參數含意**

   * `phys_addr`：要映射的**物理起始位址**。
   * `size`     ：映射空間的**大小（byte）**。
   * `mtype`    ：映射記憶體的**屬性**，可以選：

     * `MT_DEVICE`            （預設，用於 MMIO）
     * `MT_DEVICE_NONSHARED`
     * `MT_DEVICE_CACHED`
     * `MT_DEVICE_WC`（write‑combining）
   * `__arm_ioremap` 這裡固定用 `MT_DEVICE`。

4. **回傳值**

   * 回傳一支 `void __iomem *`（或 `volatile void __iomem *`）型態的指標，指向剛剛「映射後」那段**核心虛擬位址**的起始位置。
   * 之後驅動要操作硬體寄存器，就用 `readl()`/`writel()` 等 API，地址就是這支指標加上偏移。

---

總結：

> **這段是在說明 `ioremap` 在 ARM Linux 下是如何由巨集展開到 `__arm_ioremap`，最終呼叫架構相關的 `arch_ioremap_caller`，並解釋了三個參數（物理位址、映射大小、記憶體屬性）與它回傳的虛擬位址指標。**
