# 設備樹 device tree
## 1. 什麼是設備樹
1. `uboot` 啟動內核用到`zImage`、`.dtb`。通過bootz啟動內核。
    - U-Boot 如何把 kernel 和 device tree blob (DTB) 載到記憶體裡，然後啟動它
    - U-Boot 啟動 zImage（ARM kernel image）＋可選的 initrd，再加上一個 DTB。
    - 把 `zImage` 放到 DRAM 的哪裡。
    - 把 `.dtb` 放到 DRAM 的哪個位置
    ```bash
    bootz 0x8080_0000(zImage` 放到 DRAM 的哪裡)   - (initrd 位址略過（沒有 initrd，就填個 `-`）)   0x8300_0000(把 .dtb 放到 DRAM 的哪個位置)
    ```
2. 設備樹: 設備和樹。(用樹形結構去描述版級設備)
    - `.dts`: device tree source，`.dts`是文件，用一個文件去把你開發版的這些訊息給描述出來(CPU數量、記憶體機地址多少、i2c接口接了那些設備、spi接口接了那些設備、...)![alt text](../class_13/image.png)

3. 在單片機驅動裡面比如: W25QXX，SPI，速度，都是在`.c`文件寫死的。板級訊息都寫到.c裡面。導致linux內核擁腫(很胖)，因此，將板子的訊息做成獨立的格式，文件擴展名為`.dts`。
    - 一個平台或機器對應一個`.dts`。

## 2. DTS、DTB、DTC的關係
- `.dts`相當於`.c`，就是DTS源碼文件。
- DTC工具相當於 gcc編譯器，將`.dts`編譯成`.dtb`文件
- `.dtb`相當於bin文件，或可執行文件。

- 通過`make dtbs`編譯所有的`.dts`文件
    ```bash
    make dtbs
    ```
# 3. DTS 語法
1. 設備樹也有頭文件，擴展名為`.dtsi`，可以將一個SOC他的其他所有設備/平台共有的訊息提出來，最為一個通用的`.dtsi`文件。
2. DTS也是 `/`(根) 開始。
3. 從`/`根節點開始描述設備訊息
4. 在`/`根節點外有一些`&cpu`這樣的語句是**追加**
5. 節點名字，完整的要求(通常是小寫)
    - `node-name@unit-address`
    - unit-address 一般都是外設暫存器的起始地址，有時候是i2c的設備地址，或者其他含意、具體節點分析。
    設備樹裡面常常遇到如下所示節點名字:
    - `intc: interrupt-controller@00a00`
    - `標籤: 名字`: 用途是不用打一長串，用標籤就可以了

---
![alt text](image-1.png)

---
![alt text](image.png)
這段文字說明了典型的 **版本 1** Device Tree Source（DTS）檔案應該長什麼樣子，其重點可以這樣理解：

1. **版本宣告**

   ```dts
   /dts-v1/;
   ```

   這一行必須出現在檔案最前面，用來告訴 DTC（Device Tree Compiler）這是「版本 1」的 DTS 檔案；
   如果沒有這行，DTC 會把它當作「版本 0」舊格式來處理，兩者在整數格式、語法上都有些微不相容的差異。

2. **記憶體保留（memory reservations）**

   ```dts
   /memreserve/ <address> <length>;
   ```

   這種語法用來告訴內核「這一塊物理記憶體不要給任何 driver 或 kernel 使用」，會寫進最終的 DTB（device tree blob）裡的保留表。

   * `<address>`：要保留的起始位址
   * `<length>` ：要保留的長度

3. **根節點與內容**

   ```dts
   / {
       /* property 定義，例如：compatible = "vendor,board"; */
       /* 子節點，例如：uart0: serial@4000 { … }; */
   };
   ```

   * `/ { … };` 這一對斜線加大括號，代表整個 device tree 的「根節點」（root node）。
   * 在大括號裡，你可以放：

     * **屬性（properties）**：像是 `model = "MyBoard";`, `aliases { ... };` 等。
     * **子節點（child nodes）**：像是各種外設（UART、I²C、GPIO……）的節點定義。

---

**總結**：
一個完整的版本 1 DTS 檔案通常長這樣：

```dts
/dts-v1/;

/memreserve/ 0x40000000 0x01000000;   /* 例：保留 64 MiB 記憶體 */
 
/ {
    compatible = "myvendor,myboard";
    model = "MyBoard v1";

    aliases {
        serial0 = &uart0;
    };

    uart0: serial@4000 {
        compatible = "vendor,uart";
        reg = <0x4000 0x100>;
    };

    /* … 其他外設節點 … */
};
```

* **第一行** 定義版本
* **第二段** （可選）定義要保留給特殊用途的記憶體範圍
* **最後一大塊** 定義整顆板子的硬體結構（屬性＋子節點）

---
![alt text](image-2.png)

---
簡單說：**`&` 在 Device Tree（DTS）裡是「phandle 參照」運算子**，用來**指到已經定義好的節點**（很像 C 裡“指標到某節點”，但不是位址運算子）。

### 你圖裡的三種用法

1. **打開/覆寫既有節點**

```dts
&csi {
    status = "okay";
};
```

意思：找到先前有標籤 `csi:` 的節點，打開它並把 `status` 改成 `"okay"`。

2. **在屬性值裡參照別的節點（phandle + 參數）**

```dts
&clks {
    assigned-clocks = <&clks IMX6UL_CLK_PLL4_AUDIO_DIV>;
    assigned-clock-rates = <786432000>;
};
```

`<&clks …>` 裡的第一個欄位是 **phandle**（指到 `clks:` 這個 clock 控制器節點）；後面跟著此 provider 定義的 **specifier**（例如 clock ID，數量由該節點的 `#clock-cells` 決定）。

3. **為裝置指定供電來源（也是 phandle 參照）**

```dts
&cpu0 {
    arm-supply = <&reg_arm>;
    soc-supply = <&reg_soc>;
    dc-supply  = <&reg_gpio_dvfs>;
};
```

每個 `*-supply` 指向對應的 regulator 節點（`reg_arm:`, `reg_soc:` …）。

### 重點備註

* 被參照的節點必須**有標籤**（例如 `clks: clock-controller@... { ... };`），才能用 `&clks` 指到它。
* 也可用路徑參照（常見於 overlay）：`&{/soc/i2c@021a0000}`。
* `&` **不是** C 的取位址；它只是在 DTS 語法裡請 dtc 把該參照解析成 32-bit phandle 值。


