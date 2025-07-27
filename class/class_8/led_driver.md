# led_driver
1. 初始化時鐘, io, GPIO...等等
2. 

---
# 終端機結果
```bash
a0000@mcalab:/sys/firmware/devicetree/base $ cd /sys/firmware/devicetree/base
a0000@mcalab:/sys/firmware/devicetree/base $ grep -R "compatible.*rp1-pio" -n .
a0000@mcalab:/sys/firmware/devicetree/base $ grep -R "@.*pio" -n .
grep: ./aliases/gpio0: binary file matches
grep: ./aliases/pio0: binary file matches
grep: ./aliases/gpiochip10: binary file matches
grep: ./aliases/gpio1: binary file matches
grep: ./aliases/gpiochip0: binary file matches
grep: ./aliases/gpio2: binary file matches
grep: ./__symbols__/rp1_dpi_16bit_cpadhi_gpio2: binary file matches
grep: ./__symbols__/rp1_i2s1_18_21: binary file matches
grep: ./__symbols__/rp1_uart1_0_1: binary file matches
grep: ./__symbols__/rp1_dpi_24bit_gpio2: binary file matches
grep: ./__symbols__/spi5_pins: binary file matches
grep: ./__symbols__/gio_aon: binary file matches
grep: ./__symbols__/bsc_pmu_sgpio4_pins: binary file matches
grep: ./__symbols__/rp1_uart4_12_13: binary file matches
grep: ./__symbols__/rp1_dpi_16bit_gpio2: binary file matches
grep: ./__symbols__/rp1_gpclksrc1_gpio18: binary file matches
grep: ./__symbols__/rp1_i2c3_6_7: binary file matches
grep: ./__symbols__/rp1_spi5_gpio13: binary file matches
grep: ./__symbols__/uart4_ctsrts_pins: binary file matches
grep: ./__symbols__/rp1_dpi_16bit_pad666_gpio0: binary file matches
grep: ./__symbols__/pwm_aon_agpio4_pins: binary file matches
grep: ./__symbols__/rp1_dpi_16bit_cpadhi_gpio0: binary file matches
grep: ./__symbols__/rp1_uart3_ctsrts_10_11: binary file matches
grep: ./__symbols__/uart2_ctsrts_pins: binary file matches
grep: ./__symbols__/rp1_dpi_24bit_gpio0: binary file matches
grep: ./__symbols__/rp1_audio_out_12_13: binary file matches
grep: ./__symbols__/uart0_ctsrts_pins: binary file matches
grep: ./__symbols__/rp1_dpi_16bit_gpio0: binary file matches
grep: ./__symbols__/bsc_m2_sgpio4_pins: binary file matches
grep: ./__symbols__/rp1_uart0_14_15: binary file matches
grep: ./__symbols__/spi10_pins: binary file matches
grep: ./__symbols__/rp1_i2c4_34_35: binary file matches
grep: ./__symbols__/rp1_i2c6_38_39: binary file matches
grep: ./__symbols__/bsc_m1_agpio13_pins: binary file matches
grep: ./__symbols__/dpi_16bit_gpio2: binary file matches
grep: ./__symbols__/spi4_pins: binary file matches
grep: ./__symbols__/gpio: binary file matches
grep: ./__symbols__/spi10_cs_gpio1: binary file matches
grep: ./__symbols__/rp1_gpclksrc0_gpio4: binary file matches
grep: ./__symbols__/rp1_pwm1_gpio45: binary file matches
grep: ./__symbols__/dpi_16bit_gpio0: binary file matches
grep: ./__symbols__/dpi_18bit_cpadhi_gpio2: binary file matches
grep: ./__symbols__/i2c3_pins: binary file matches
grep: ./__symbols__/uart4_pins: binary file matches
grep: ./__symbols__/spi3_pins: binary file matches
grep: ./__symbols__/rp1_gpclksrc0_gpio20: binary file matches
grep: ./__symbols__/dpi_18bit_cpadhi_gpio0: binary file matches
grep: ./__symbols__/usb_vbus_pins: binary file matches
grep: ./__symbols__/rp1_i2c0_0_1: binary file matches
grep: ./__symbols__/dpi_gpio1: binary file matches
grep: ./__symbols__/rp1_dpi_hvsync: binary file matches
grep: ./__symbols__/rp1_i2c1_10_11: binary file matches
grep: ./__symbols__/rp1_i2c3_14_15: binary file matches
grep: ./__symbols__/rp1_i2c3_22_23: binary file matches
grep: ./__symbols__/i2c2_pins: binary file matches
grep: ./__symbols__/rp1_sdio0_22_27: binary file matches
grep: ./__symbols__/rp1_dpi_18bit_gpio2: binary file matches
grep: ./__symbols__/uart3_pins: binary file matches
grep: ./__symbols__/spi2_pins: binary file matches
grep: ./__symbols__/rp1_i2c2_4_5: binary file matches
grep: ./__symbols__/dpi_16bit_cpadhi_gpio2: binary file matches
grep: ./__symbols__/spi0_cs_pins: binary file matches
grep: ./__symbols__/pwm_aon_agpio1_pins: binary file matches
grep: ./__symbols__/rp1_spi1_gpio19: binary file matches
grep: ./__symbols__/rp1_uart1_ctsrts_2_3: binary file matches
grep: ./__symbols__/rp1_uart2_ctsrts_6_7: binary file matches
grep: ./__symbols__/rp1_i2c0_8_9: binary file matches
grep: ./__symbols__/rp1_dpi_18bit_gpio0: binary file matches
grep: ./__symbols__/uart3_ctsrts_pins: binary file matches
grep: ./__symbols__/i2c3_m4_agpio0_pins: binary file matches
grep: ./__symbols__/i2c1_pins: binary file matches
grep: ./__symbols__/dpi_18bit_gpio2: binary file matches
grep: ./__symbols__/pwm_aon_agpio7_pins: binary file matches
grep: ./__symbols__/uart1_ctsrts_pins: binary file matches
grep: ./__symbols__/uart2_pins: binary file matches
grep: ./__symbols__/dpi_16bit_cpadhi_gpio0: binary file matches
grep: ./__symbols__/rp1_i2c5_44_45: binary file matches
grep: ./__symbols__/dpi_18bit_gpio0: binary file matches
grep: ./__symbols__/rp1_uart3_8_9: binary file matches
grep: ./__symbols__/rp1_dpi_18bit_cpadhi_gpio2: binary file matches
grep: ./__symbols__/rp1_i2s0_18_21: binary file matches
grep: ./__symbols__/i2c0_pins: binary file matches
grep: ./__symbols__/rp1_spi2_gpio1: binary file matches
grep: ./__symbols__/gio: binary file matches
grep: ./__symbols__/rp1_spi8_cs_gpio52: binary file matches
grep: ./__symbols__/uart1_pins: binary file matches
grep: ./__symbols__/spi0_pins: binary file matches
grep: ./__symbols__/rp1_dpi_18bit_cpadhi_gpio0: binary file matches
grep: ./__symbols__/rp1_gpio: binary file matches
grep: ./__symbols__/rp1_uart0_ctsrts_16_17: binary file matches
grep: ./__symbols__/rp1_spi0_gpio9: binary file matches
grep: ./__symbols__/rp1_spi8_gpio49: binary file matches
grep: ./__symbols__/rp1_sdio1_28_33: binary file matches
grep: ./__symbols__/pio: binary file matches
grep: ./__symbols__/rp1_uart4_ctsrts_14_15: binary file matches
grep: ./__symbols__/rp1_gpclksrc1_gpio21: binary file matches
grep: ./__symbols__/rp1_uart2_4_5: binary file matches
grep: ./__symbols__/rp1_spi3_gpio5: binary file matches
grep: ./__symbols__/spi10_cs_pins: binary file matches
grep: ./__symbols__/rp1_pio: binary file matches
grep: ./__symbols__/spi10_gpio2: binary file matches
grep: ./__symbols__/rp1_i2c2_12_13: binary file matches
grep: ./__symbols__/rp1_gpclksrc1_gpio5: binary file matches
grep: ./__symbols__/dpi_gpio0: binary file matches
grep: ./__symbols__/rp1_i2c1_2_3: binary file matches
grep: ./__symbols__/rp1_spi4_gpio9: binary file matches
grep: ./__symbols__/rp1_i2c4_40_41: binary file matches
grep: ./__symbols__/uart0_pins: binary file matches
grep: ./__symbols__/rp1_spi0_cs_gpio7: binary file matches
grep: ./__symbols__/rp1_dpi_16bit_pad666_gpio2: binary file matches
a0000@mcalab:/sys/firmware/devicetree/base $ cat __symbols__/rp1_pio | tr '\0' '\n'
/axi/pcie@1000120000/rp1/pio@178000
a0000@mcalab:/sys/firmware/devicetree/base $ cd /sys/firmware/devicetree/base/axi/pcie@1000120000/rp1/pio@178000
a0000@mcalab:/sys/firmware/devicetree/base/axi/pcie@1000120000/rp1/pio@178000 $ hexdump -n 16 -e '4/4 "0x%08x\n"' reg
0xc0000000
0x00801740
0x00000000
0x20000000
```
```bash

```
---
## 補充說明

**Linux kernel 下的“MMIO（memory-mapped I/O）”存取**，重點是：

1. **I/O 概念**

   * 這裡說的 I/O 不是單片機的 GPIO，而是「外部硬體暫存器」映射到 CPU 空間的概念。
   * 如果映射到 x86 的 “I/O 空間” 就叫 I/O 端口（port‐I/O）；映射到一般記憶體空間就叫 I/O **内存**（MMIO）。
   * 在 ARM（包含 Raspberry Pi）上，只有「I/O 内存」這回事，直接把硬體暫存器當記憶體用。

2. **ioremap 映射後**

   * 用 `ioremap(phys_addr, size)` 把硬體暫存器的**物理地址**映成**內核虛擬地址**，取得一個 `__iomem` 指標。
   * 雖然掛好了指標，你可以用 `*(volatile … __iomem *)` 直接讀寫，但 **不建議**，應該改用專門的 I/O 存取函式。

3. **I/O 存取函式**

   ```c
   u8  readb(const volatile void __iomem *addr);   // 8-bit 讀
   u16 readw(const volatile void __iomem *addr);   // 16-bit 讀
   u32 readl(const volatile void __iomem *addr);   // 32-bit 讀

   void writeb(u8  val, volatile void __iomem *addr);  // 8-bit 寫
   void writew(u16 val, volatile void __iomem *addr);  // 16-bit 寫
   void writel(u32 val, volatile void __iomem *addr);  // 32-bit 寫
   ```

   * `addr` 就是 `ioremap()` 回傳的那個 `__iomem` 指標＋offset。
   * 這些函式底層會做必要的 memory barrier／匯流排同步，確保硬體寫入不被 compiler 或 CPU reorder。

---

> **總結**：
>
> * **映射（ioremap）後**，要用 `read*/write*` 系列函式來操作 MMIO，
> * 不是直接 `*ptr`，也不是用標準 C 的 `memcpy`／`*(volatile…)`，
> * 這樣才能保證在 Linux driver／ARM 上正確、安全地跟硬體暫存器溝通。

---
Pi 5 上的 Arm CPU 本身是 **64-bit** 的（Armv8-A/AArch64 架構），但是大部分外部暫存器都是 **32-bit wide**，也就是你用 `readl()/writel()` 存取的都是 32-bit 寄存器。

* **CPU 架構**：Cortex-A76 (Armv8-A)，在 Linux 上跑的是 aarch64，所有指標（包括 `__iomem *`）都是 64-bit 寬度。
* **MMIO 寄存器**：GPIO、UART、SPI… 這類外設一次傳輸都以 32-bit 為單位，所以用 `readl()`/`writel()`。

換句話說──

* 指標大小 (pointer width) = 64 位元
* 寄存器存取寬度 = 32 位元

---
**`__iomem` 重點整理**

* **本質：**

  * 不是 C 語言的「新型別」，而是一個給 Linux kernel 靜態分析工具 **sparse** 用的「屬性註記（attribute annotation）」
  * 在宣告指標時加上，告訴工具「這個指標指向 MMIO（I/O memory）位址空間」

* **定義（大致長這樣）**

  ```c
  #define __iomem __attribute__((noderef, address_space(2)))
  ```

  * `address_space(2)`：第 2 類記憶體空間（I/O）
  * `noderef`：禁止用 `*ptr` 直接解參考

* **為什麼要用？**

  1. **安全檢查**：防止誤用一般記憶體存取方法（`*ptr = val`）去操作 MMIO
  2. **強制使用 IO API**：必須用 `readb()/readl()`、`writeb()/writel()` 這類函式，才會被認可

* **正確用法示例**

  ```c
  void __iomem *base = ioremap(0x60004000, SZ_4K);
  u32 v = readl(base + 0x04);              // 讀 GPFSEL1
  writel(v | (1 << 17), base + 0x04);      // 設 GPIO17 為 output
  iounmap(base);
  ```

* **錯誤示例（sparse 會警告）**

  ```c
  void __iomem *base = …;
  *base = 0x12345678;   // ❌ 直接解參考，不能用
  ```

* **與其他標註比較**

  | 標註         | 用途                  | 需搭配 API                         |
  | ---------- | ------------------- | ------------------------------- |
  | `__user`   | 指向 user-space 記憶體   | copy\_to\_user/copy\_from\_user |
  | `__kernel` | 指向 kernel-space 記憶體 | 直接 `*ptr`                       |
  | `__iomem`  | 指向 I/O memory（MMIO） | read\*/write\*                  |

---

> 簡言之， **`__iomem` 是一個屬性註記**，讓你在 Linux kernel driver 裡 **安全且正確** 地存取「記憶體映射 I/O」。

---
**`ioremap()` 重點歸納**

1. **功能**

   * 在 Linux kernel 中，將「物理位址範圍」映射到「內核虛擬位址空間」
   * 讓 driver 能以指標方式存取 MMIO（memory-mapped I/O）暫存器

2. **原型**

   ```c
   void __iomem *ioremap(resource_size_t phys_addr, unsigned long size);
   ```

   * `phys_addr`：起始物理位址
   * `size`：要映射的位元組長度（通常至少一頁，常見 `SZ_4K` 或 `PAGE_SIZE`）

3. **回傳值**

   * 成功：回傳對應的 **`__iomem`** 指標，用於後續 `read*/write*` 粗存取
   * 失敗：回傳 `NULL`，driver 應檢查並錯誤回報

4. **存取方式**

   * **絕對禁止** `*ptr` 直接讀寫
   * 必須用 `readb()/readw()/readl()`、`writeb()/writew()/writel()` 等 API

5. **對應解除**

   ```c
   iounmap(void __iomem *addr);
   ```

   * driver 卸載或不再使用時，呼叫 `iounmap()` 來釋放映射

6. **實務建議**

   * **至少映射一頁**（4KiB）以上，以涵蓋所有暫存器 offset
   * 映射大小寫 `SZ_4K`、`SZ_16K`…方便可讀
   * 在 probe/init 階段做 `ioremap()`，在 remove/exit 階段做 `iounmap()`

---

> 簡而言之，**`ioremap()`** 是 Linux driver 用來「橋接」物理 MMIO 與內核虛擬位址的重要函式，配合 `__iomem` 標註與 `read*/write*` API，才算完整且安全的 MMIO 存取流程。

---
**`iounmap()` 重點歸納**

1. **功能**

   * 解除之前用 `ioremap()` 建立的 MMIO 映射
   * 釋放內核虛擬地址空間與底層頁表資源

2. **原型**

   ```c
   void iounmap(void __iomem *addr);
   ```

   * `addr`：先前 `ioremap()` 回傳的指標

3. **使用時機**

   * **module remove**／**driver unload** 階段
   * 不再存取那段 MMIO 之前必須呼叫，否則可能造成資源洩漏或記憶體衝突

4. **搭配流程**

   ```c
   /* probe 或 init 階段 */
   base = ioremap(PHYS_ADDR, SIZE);

   /* 用 readl()/writel() 等存取 */

   /* remove 或 exit 階段 */
   iounmap(base);
   ```

5. **注意事項**

   * **只能對等量 ioremap/iounmap**：每次 ioremap 一定要有對應的 iounmap
   * **addr 必須與 ioremap 回傳值相符**：不要解偏移後再 unmap
   * 釋放後切勿再使用該指標

6. **實務建議**

   * 在 `platform_driver` 的 `.remove` callback 或 `module_exit` 中呼叫
   * 若同時映射多段 MMIO，各自保存指標並分別 unmap

---

> 簡言之，**`iounmap()`** 就是清除前面 `ioremap()` 映射的 MMIO，確保 driver 安全地釋放記憶體與頁表資源。

---