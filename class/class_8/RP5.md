下面是你在 `/sys/firmware/devicetree/base` 底下為了找出 RP1 PIO block 所做的步驟，以及最終拿到的 `reg` 屬性值，並對它們的意義做詳細歸納：

---

## 1. 瀏覽 device-tree 目錄

```bash
cd /sys/firmware/devicetree/base
```

## 2. 嘗試用 `grep` 找 `rp1-pio`

```bash
grep -R "compatible.*rp1-pio" -n .
# 沒有任何輸出，代表 node 上沒有直接標示 “compatible = rp1-pio”。
```

## 3. 廣泛搜尋所有含 “`@...pio`” 的檔名／符號

```bash
grep -R "@.*pio" -n .
# 在 __symbols__ 目錄底下，找到：__symbols__/rp1_pio
```

## 4. 讀出符號檔案，確認對應的 node 路徑

```bash
cat __symbols__/rp1_pio | tr '\0' '\n'
# 輸出：/axi/pcie@1000120000/rp1/pio@178000
```

## 5. 進入該 node，讀取它的 reg 屬性

```bash
cd axi/pcie@1000120000/rp1/pio@178000
hexdump -n 16 -e '4/4 "0x%08x\n"' reg
```

**輸出**：

```
0xc0000000
0x00801740
0x00000000
0x20000000
```

---

## 6. `reg` 屬性的結構與意義

在 PCIe-橋接下，這個 `reg` 屬性一共由四個 32-bit big-endian cell 組成，分別是：

| cell                    | 含義                                          |
| ----------------------- | ------------------------------------------- |
| **cell 0** = 0xc0000000 | Base address 高 32 bits （大端）                 |
| **cell 1** = 0x00801740 | Base address 低 32 bits                      |
| **cell 2** = 0x00000000 | Region size 高 32 bits                       |
| **cell 3** = 0x20000000 | Region size 低 32 bits = 0x20000000 (512 MB) |

* **完整 64-bit 基址** = `0xc0000000_00801740`
* **Region 大小** = `0x00000000_20000000` = 512 MB

Linux 驅動要用到的通常是這個「CPU 視角下的物理位址」，也就是上面合成後的 64-bit 基址。若只取低 32 bit，則是 `0x00801740`；若你的驅動支援 64-bit お通，用全 64-bit 會比較正確。

---

### RP1 上 PIO（pio\@178000）的物理基址如何「頁對齊」，步驟如下：

1. **從 Device Tree 看寄存器物理位址**
   在 DTS 裡，PIO 節點可能長這樣：

   ```dts
   /axi/pcie@…/rp1/pio@178000 {
       reg = <0xc0000000 0x00801740  0x00000000 0x20000000>;
       …
   };
   ```

   這裡 `<0xc0000000 0x00801740>` 表示：

   * PCIe BAR 的物理基址是 `0xc0000000`
   * 在這個區域內，PIO 寄存器區塊相對偏移 `0x00801740`
     → 真正的物理起始地址就等於

   ```
   0xc0000000 + 0x00801740 = 0xc0801740
   ```

2. **為何要做 4 KiB 頁對齊映射？**
   `ioremap()` 只能以「頁大小」(4 KiB, 0x1000) 來映射物理記憶體，所以不能只映射 `0xc0801740` 這個小位址範圍。必須先把它往下取整到附近的 4 KiB 邊界，也就是

   ```c
   GPIO_BASE_PHYS = 0xc0801740 & ~(0x1000 - 1);  // = 0xc0801000
   ```

   然後呼叫

   ```c
   ioremap(GPIO_BASE_PHYS, 0x1000);
   ```

   這樣一次就把 `0xc0801000`～`0xc0810FFF` 的整頁映射到 kernel 空間，其中偏移 `0x740` 的位置正好對應到 PIO 寄存器區塊。
