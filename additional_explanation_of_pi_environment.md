# 樹梅派環境的補充說明
## 1. make olddefconfig

* **作用**：把你從 `/boot/config-$(uname -r)` 拷過來的 `.config` 裡，新出現的選項都用預設值填好。
* **產出**：更新過的 `.config`，**不**會產生任何 .o/.ko。

## 2. make -j\$(nproc) modules\_prepare

* **作用**：為編譯外部模組產生必要的頭檔與腳本，但不真的去編譯驅動。
* **產出**：

  * `include/generated/autoconf.h`
  * `include/config/auto.conf`
  * `scripts/mod/modpost`（以及相關的 scripts/mod/\*.o，modpost 工具本身）
  * 各種 Kbuild 腳本和 wrapper（scripts/Makefile、scripts/\*.sh…）

## 3. （可選）make -j\$(nproc) modules

* **作用**：編譯 kernel tree 裡面所有標記為 module 的程式碼。
* **產出**：大量的 `.ko` 檔（drivers/…/\*\*/\*.ko），也會更新 `Module.symvers`。

## 4. scripts/mod/modpost -m Module.symvers

* **作用**：呼叫剛編好的 `modpost` 工具，掃描所有已編譯的模組（.ko）中的導出符號，生成一份全內核的符號表。
* **產出**：`Module.symvers`，裡面列著每個 `EXPORT_SYMBOL` 的版本資訊。

---

### 簡單總結

* **modules\_prepare** 階段編譯出的是**模組建置基礎工具**（像是 modpost）和**自動產生的頭檔**，還沒有編你的驅動。
* 如果你接著跑 `make modules`，才真的會編出 kernel 裡內建模組的 `.ko`。
* 最後用 `modpost -m` 生成的 `Module.symvers`，就是你在 char\_demo 的 Makefile 裡 `KBUILD_EXTRA_SYMBOLS` 要用的符號表。

所以，“這邊”真正被 gcc 編譯的，是 **modpost 這個工具**（以及它需要的 scripts/mod/\*.o），和在你執行 `make modules` 時才會出現的各種內核模組（.ko）。要編出自己的 `hello_char.ko`，還是要回到 char\_demo 執行 `make`（也就是 `make -C /lib/modules/... M=$(PWD) modules`）。

---
# zImage 和 .dtb

在樹莓派（以及大多數 ARM 平台）上，`zImage` 和各種 `.dtb` 檔案是開機過程中「引導程式（bootloader）」要讀取的兩個關鍵檔案：

1. **`zImage`（Compressed Kernel Image）**

   * 這是「壓縮後」的 Linux 核心映像檔（kernel image）。
   * 引導程式會先把它載入記憶體，然後解壓縮（inflate）成真正的執行態核心（vmlinux）。
   * 之所以用壓縮形式，一方面可以減少在儲存媒體／記憶體佔用，另一方面在早期 ARM 架構板子上也必須先用一小段解壓程式把核心載入。
   * 在 Raspberry Pi 上，這個 `zImage` 通常會放在 SD 卡的 `/boot` 分割區，並在 `config.txt` 用 `kernel=zImage` 指定。

2. **`.dtb`（Device Tree Blob）**

   * Device Tree 是一種用來描述硬體拓撲與配備的資料結構（例如：CPU 型號、記憶體大小、外設中斷號、GPIO 分腳對應、時脈來源…等）。
   * DTS（.dts）是「原始碼」格式，編譯後就變成二進位的 `.dtb`（Device Tree Blob）。
   * 核心啟動時，會搭配這份 `.dtb`，知道板子上到底有哪些外設／如何初始化它們，不需要把所有驅動都寫死在核心裡。
   * Raspberry Pi 標準做法是把多個型號對應的 `.dtb`（例如 `bcm2711-rpi-4-b.dtb`、`bcm2711-rpi-5-b.dtb`）放在 `/boot`，再在 `config.txt` 用 `device_tree=` 或透過 `dtoverlay=` 選擇要載入哪一個。

---

### 為什麼要自己編譯出 zImage 與 .dtb？

* **版本對應性**：你下載的樹莓派官方韌體附帶的核心（zImage）和 DTB 版本，可能和你自己從 GitHub 抓的 kernel tree 分支（例如 `rpi-6.12.y`）不一樣。為了保證 out-of-tree 模組 (hello\_char.ko) 能和核心版本、符號表一一對應，你需要用相同的原始碼編出 zImage、modules\_prepare 時生成的符號表，還要配上對應的 .dtb。
* **測試自訂選項**：如果你在 kernel config（`.config`） 裡打開或關閉了某些選項、或更新了驅動程式才能正確測試，也要自己編 kernel。

---

#### 總結

* **zImage**＝「壓縮後的核心執行檔」，引導程式載入並解壓縮後才是真正跑起來的 Linux 核心。
* **.dtb**＝「硬體描述二進位檔」，告訴核心它跑在哪塊板子上，要怎麼初始化記憶體、外設、時脈與中斷等。

了解這兩個檔案的角色之後，你就能把它們一起放到 SD 卡 `/boot`，配合你的驅動模組進行完整測試了。

---