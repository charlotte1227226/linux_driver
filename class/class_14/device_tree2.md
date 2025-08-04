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


---


---
## 補充說明

在類 Unix 系統（像 Linux、macOS）或類似架構的作業系統裡，`bin`（binary 的簡寫）資料夾主要是用來放「執行檔」（binaries），也就是你可以直接在終端機呼叫、執行的程式。依照不同的位置，通常分成幾種常見的 `bin` 目錄：

| 目錄               | 內容                                 | 用途小結                  |
| ---------------- | ---------------------------------- | --------------------- |
| `/bin`           | 基本系統指令的執行檔，例如 `ls`、`cp`、`mv`       | 啟動系統必備，單用最小系統也能運作     |
| `/usr/bin`       | 大多數使用者命令的執行檔，例如 `gcc`、`python`     | 系統安裝的大部分程式都放這裡        |
| `/usr/local/bin` | 使用者或系統管理員自行編譯／安裝的程式執行檔             | 安裝第三方軟體時，通常放在這裡避免覆寫   |
| `/sbin`          | 系統管理或維護用的執行檔，例如 `fdisk`、`ifconfig` | 只有 root（或有特權的使用者）才需要用 |
| `/usr/sbin`      | 額外的系統管理工具，例如 `sshd`、`httpd`        | 同樣只給有權限的帳號執行          |

---

## 為什麼要分這麼多個 `bin`？

1. **安全與權限分離**

   * 一般使用者（non-root）平常只需要 `/bin`、`/usr/bin` 下的工具。
   * 涉及系統設定、硬體管理的工具才放在 `/sbin`、`/usr/sbin`，避免誤用或被攻擊者利用。

2. **最小系統（rescue mode）可用性**

   * 在系統維護或救援模式時，只有最基本的檔案系統已掛載，仍要能執行如 `ls`、`mount`、`cp` 等指令，所以這些工具都放到 `/bin`。

3. **避免更新衝突**

   * 系統套件更新時會影響 `/usr/bin`，使用者自己編譯的程式就放到 `/usr/local/bin`，減少被覆寫或套件衝突的機會。

---

## 常見指令查路徑範例

假設你想知道某個指令到底在 `哪個 bin 目錄`，可以這樣做：

```bash
which ls
# 可能輸出：/bin/ls

which python3
# 可能輸出：/usr/bin/python3

which my_custom_tool
# 如果你把它放在 /usr/local/bin，則會輸出：/usr/local/bin/my_custom_tool
```

`which` 會在你的 `$PATH` 環境變數所指定的路徑裡，依序搜尋出第一個符合的可執行檔。

---

### 小結

* **`bin`** 資料夾存放「可執行檔（binaries）」。
* 依照系統需要、權限分級及安裝來源，常見有 `/bin`、`/usr/bin`、`/usr/local/bin`、`/sbin`、`/usr/sbin`。
* 了解這些目錄的分工，有助於你管理系統、避免檔案衝突，也能更快速地排錯與設定環境變數。

---
以下是從最基礎到完成的完整流程，幫你把「撰寫／編譯 Device Tree Source（.dts）並讓系統載入」的各種情境一併歸納：

---

## 一、純手動編譯單一 .dts → .dtb（最輕量）

1. **安裝 Device Tree Compiler**

   ```bash
   sudo apt update
   sudo apt install device-tree-compiler
   ```

2. **撰寫你的 DTS**

   * 建立一個文字檔 `myboard.dts`，內容只包含你要描述或覆蓋的節點。例如：

     ```dts
     /dts-v1/;
     / {
       model = "My Custom Board";
       compatible = "brcm,bcm2711";

       mydevice@12340000 {
         compatible = "myvendor,mydevice";
         reg = <0x12340000 0x1000>;
       };
     };
     ```

3. **編譯**

   ```bash
   dtc -I dts -O dtb -o myboard.dtb myboard.dts
   ```

   * `-I dts`：輸入是 DTS
   * `-O dtb`：輸出是 DTB

4. **安裝到開機分區**

   ```bash
   sudo cp myboard.dtb /boot/firmware/    # 或 /boot/
   ```

5. **（如有需要）在 `/boot/firmware/config.txt` 強制指定**

   ```ini
   device_tree=myboard.dtb
   ```

6. **重開機、驗證**

   ```bash
   sudo reboot
   dmesg | grep -i raspberry\ pi       # 確認 Machine model
   cat /proc/device-tree/model          # 應顯示 My Custom Board
   ```

---

## 二、編譯 Overlay (.dts → .dtbo) — 只動小修改或外掛板

1. **撰寫 Overlay DTS**

   ```dts
   /dts-v1/;
   /plugin/;

   / {
     compatible = "brcm,bcm2711";

     fragment@0 {
       target = <&i2c1>;
       __overlay__ {
         status = "okay";
       };
     };
   };
   ```

2. **編成 DTBO**

   ```bash
   dtc -@ -I dts -O dtb -o myoverlay.dtbo myoverlay.dts
   ```

3. **複製到 overlays 目錄**

   ```bash
   sudo cp myoverlay.dtbo /boot/firmware/overlays/  # 或 /boot/overlays/
   ```

4. **在 config.txt 啟用**

   ```ini
   dtoverlay=myoverlay
   ```

5. **重開機→檢查**

   ```bash
   ls /proc/device-tree/overlays
   cat /proc/device-tree/overlays/myoverlay/status  # 應回 okay
   ```

---

## 三、若要重編官方板子 .dtb（或大量 overlays）

> 當你需要修改或更新官方 `.dts`（例如 bcm2712-rpi-5-b.dts），必須有完整 kernel 原始碼樹。

1. **取得原始碼**

   * **方式 A（淺層 clone 指定分支）**

     ```bash
     git clone --depth 1 --branch rpi-6.12.y \
       https://github.com/raspberrypi/linux.git \
       raspberrypi-linux
     ```
   * **方式 B（apt 套件）**

     ```bash
     sudo apt install raspberrypi-kernel-source
     cd /usr/src/linux-headers-$(uname -r)
     ```

2. **進入原始碼根目錄**

   ```bash
   cd ~/raspberrypi-linux   # 或 /usr/src/…/build
   ```

3. **匯入現用 config（確保選項一致）**

   ```bash
   cp /boot/firmware/config-$(uname -r) .config
   make ARCH=arm64 olddefconfig
   ```

4. **編譯所有 DTB**

   ```bash
   make ARCH=arm64 dtbs
   ```

   * 結果會在：

     ```
     arch/arm64/boot/dts/broadcom/*.dtb
     arch/arm64/boot/dts/overlays/*.dtbo
     ```

5. **複製到開機分區**

   ```bash
   sudo cp arch/arm64/boot/dts/broadcom/bcm2712-rpi-5-b.dtb /boot/firmware/
   sudo cp arch/arm64/boot/dts/overlays/*.dtbo       /boot/firmware/overlays/
   ```

6. **（如有需要）在 config.txt 指定**

   ```ini
   device_tree=bcm2712-rpi-5-b.dtb
   dtoverlay=your-overlay
   ```

7. **重開機 & 驗證**

   ```bash
   sudo reboot
   dmesg | grep -i of: 
   cat /proc/device-tree/model
   ls /proc/device-tree/overlays
   ```

---

### 四、常見檢查點

* **路徑**

  * DTS/DTB：`/boot/firmware/`（Ubuntu Pi）或 `/boot/`（Raspbian）
  * Overlays：`/boot/firmware/overlays/` 或 `/boot/overlays/`

* **檔名在 config.txt**

  * `device_tree=` 不要加 `.dtb` 副檔名
  * `dtoverlay=` 不要加 `.dtbo` 副檔名

* **查看運行中 DT**

  ```bash
  cat /proc/device-tree/model
  sudo dtc -I fs -O dts /proc/device-tree -o running.dts
  ```

* **回滾**
  把原版 `.dtb.orig` 複製回去，移掉 config.txt 的 overlay/ device\_tree 設定，重開機。

---

以上即為「從頭到尾」的三種主要流程：

1. **純手動 DTS→DTB**（最簡單）
2. **Overlay 小修**（方便外掛板、切功能）
3. **完整 kernel source → make dtbs**（全面改官方 DTS）

依你的需求選擇對應流程，就能快速又乾淨地完成 Device Tree 的編譯與部署。

---
# 終端機結果
```bash
a0000@mcalab:~/Desktop/linux_driver/class $ cd /
a0000@mcalab:/ $ ls
bin  boot  dev  etc  home  lib  lost+found  media  mnt  opt  proc  root  run  sbin  srv  sys  tmp  usr  var
a0000@mcalab:/ $ cd /boot/
a0000@mcalab:/boot $ ls
cmdline.txt                  config-6.12.34+rpt-rpi-2712  firmware                         initrd.img-6.12.34+rpt-rpi-2712  overlays                         System.map-6.12.34+rpt-rpi-2712  vmlinuz-6.12.25+rpt-rpi-v8
config-6.12.25+rpt-rpi-2712  config-6.12.34+rpt-rpi-v8    initrd.img-6.12.25+rpt-rpi-2712  initrd.img-6.12.34+rpt-rpi-v8    System.map-6.12.25+rpt-rpi-2712  System.map-6.12.34+rpt-rpi-v8    vmlinuz-6.12.34+rpt-rpi-2712
config-6.12.25+rpt-rpi-v8    config.txt                   initrd.img-6.12.25+rpt-rpi-v8    issue.txt                        System.map-6.12.25+rpt-rpi-v8    vmlinuz-6.12.25+rpt-rpi-2712     vmlinuz-6.12.34+rpt-rpi-v8
a0000@mcalab:/boot $ ls -l *.dtb
ls: cannot access '*.dtb': No such file or directory
a0000@mcalab:/boot $ cd ~/linux
bash: cd: /home/a0000/linux: No such file or directory
a0000@mcalab:/boot $ ls /lib/modules/$(uname -r)/build
arch  include  Makefile  Module.symvers  scripts  tools
a0000@mcalab:/boot $ cd /lib/modules/$(uname -r)/build
a0000@mcalab:/lib/modules/6.12.34+rpt-rpi-2712/build $ ls arch/arm64/boot/dts/broadcom/*.dtb
ls: cannot access 'arch/arm64/boot/dts/broadcom/*.dtb': No such file or directory
a0000@mcalab:/lib/modules/6.12.34+rpt-rpi-2712/build $ cd ~
a0000@mcalab:~ $ ls
Bookshelf  Desktop  Documents  Downloads  kernel-src  Music  Pictures  Public  Templates  Videos
a0000@mcalab:~ $ whichcore
bash: whichcore: command not found
a0000@mcalab:~ $ whichcpu
bash: whichcpu: command not found
a0000@mcalab:~ $ git clone --depth=1 https://github.com/raspberrypi/linux.git raspberrypi-linux
Cloning into 'raspberrypi-linux'...
remote: Enumerating objects: 92772, done.
remote: Counting objects: 100% (92772/92772), done.
remote: Compressing objects: 100% (82452/82452), done.
remote: Total 92772 (delta 9588), reused 87868 (delta 9332), pack-reused 0 (from 0)
Receiving objects: 100% (92772/92772), 258.25 MiB | 5.50 MiB/s, done.
Resolving deltas: 100% (9588/9588), done.
Updating files: 100% (87551/87551), done.
a0000@mcalab:~ $ cd raspberrypi-linux
a0000@mcalab:~/raspberrypi-linux $ git fetch --all --tags
remote: Enumerating objects: 11200234, done.
remote: Counting objects: 100% (11200215/11200215), done.
remote: Compressing objects: 100% (1731046/1731046), done.
^Cceiving objects:  36% (4088246/11130755), 2.23 GiB | 8.20 MiB/s   
a0000@mcalab:~/raspberrypi-linux $ ls /..git
ls: cannot access '/..git': No such file or directory
a0000@mcalab:~/raspberrypi-linux $ lss
bash: lss: command not found
a0000@mcalab:~/raspberrypi-linux $ ls
arch   certs    CREDITS  Documentation  fs       init      ipc     Kconfig  lib       MAINTAINERS  mm   README     rust     scripts   sound  usr
block  COPYING  crypto   drivers        include  io_uring  Kbuild  kernel   LICENSES  Makefile     net  README.md  samples  security  tools  virt
a0000@mcalab:~/raspberrypi-linux $ ls .git
branches  config  description  FETCH_HEAD  HEAD  hooks  index  info  logs  objects  packed-refs  refs  shallow
a0000@mcalab:~/raspberrypi-linux $ cd .git
a0000@mcalab:~/raspberrypi-linux/.git $ cd objects/
a0000@mcalab:~/raspberrypi-linux/.git/objects $ ls
info  pack
a0000@mcalab:~/raspberrypi-linux/.git/objects $ cd pack/
a0000@mcalab:~/raspberrypi-linux/.git/objects/pack $ ls
pack-284b5e6a1812bd6a0cd3d60fafb19f430ea894c6.idx  pack-284b5e6a1812bd6a0cd3d60fafb19f430ea894c6.pack  tmp_pack_4NX3at
a0000@mcalab:~/raspberrypi-linux/.git/objects/pack $ cd ../../..
a0000@mcalab:~/raspberrypi-linux $ cd -
/home/a0000/raspberrypi-linux/.git/objects/pack
a0000@mcalab:~/raspberrypi-linux/.git/objects/pack $ rm tmp_pack_*
rm: remove write-protected regular file 'tmp_pack_4NX3at'? y
a0000@mcalab:~/raspberrypi-linux/.git/objects/pack $ ls
pack-284b5e6a1812bd6a0cd3d60fafb19f430ea894c6.idx  pack-284b5e6a1812bd6a0cd3d60fafb19f430ea894c6.pack
a0000@mcalab:~/raspberrypi-linux/.git/objects/pack $ cd ../..
a0000@mcalab:~/raspberrypi-linux/.git $ cd ..
a0000@mcalab:~/raspberrypi-linux $ git fetch --depth=1 origin rpi-6.12.y
remote: Total 0 (delta 0), reused 0 (delta 0), pack-reused 0 (from 0)
From https://github.com/raspberrypi/linux
 * branch                rpi-6.12.y -> FETCH_HEAD
a0000@mcalab:~/raspberrypi-linux $ git checkout -b rpi-6.12.y origin/rpi-6.12.y
fatal: a branch named 'rpi-6.12.y' already exists
a0000@mcalab:~/raspberrypi-linux $ git checkout rpi-6.12.y
Already on 'rpi-6.12.y'
Your branch is up to date with 'origin/rpi-6.12.y'.
a0000@mcalab:~/raspberrypi-linux $ ls arch/arm64/boot/dts/broadcom
bcm2710-rpi-2-b.dts       bcm2710-rpi-cm3.dts       bcm2711-rpi-4-b.dts     bcm2712d0-rpi-5-b.dts  bcm2712-rpi-500.dts        bcm2712-rpi-cm5-cm5io.dts   bcm2712-rpi-cm5l-cm5io.dts  bcm2837-rpi-3-b.dts       bcmbca      stingray
bcm2710-rpi-3-b.dts       bcm2710-rpi-zero-2.dts    bcm2711-rpi-cm4.dts     bcm2712-d-rpi-5-b.dts  bcm2712-rpi-5-b.dts        bcm2712-rpi-cm5.dtsi        bcm2712-rpi-cm5l.dtsi       bcm2837-rpi-3-b-plus.dts  Makefile
bcm2710-rpi-3-b-plus.dts  bcm2710-rpi-zero-2-w.dts  bcm2711-rpi-cm4-io.dts  bcm2712-ds.dtsi        bcm2712-rpi-cm4io.dtsi     bcm2712-rpi-cm5io.dtsi      bcm2712-rpi.dtsi            bcm2837-rpi-cm3-io3.dts   northstar2
bcm2710-rpi-cm0.dts       bcm2711-rpi-400.dts       bcm2711-rpi-cm4s.dts    bcm2712.dtsi           bcm2712-rpi-cm5-cm4io.dts  bcm2712-rpi-cm5l-cm4io.dts  bcm2837-rpi-3-a-plus.dts    bcm2837-rpi-zero-2-w.dts  rp1.dtsi
a0000@mcalab:~/raspberrypi-linux $ cp /boot/config-$(uname -r) .config
a0000@mcalab:~/raspberrypi-linux $ make ARCH=arm64 olddefconfig
  HOSTCC  scripts/basic/fixdep
  HOSTCC  scripts/kconfig/conf.o
  HOSTCC  scripts/kconfig/confdata.o
  HOSTCC  scripts/kconfig/expr.o
  LEX     scripts/kconfig/lexer.lex.c
  YACC    scripts/kconfig/parser.tab.[ch]
  HOSTCC  scripts/kconfig/lexer.lex.o
  HOSTCC  scripts/kconfig/menu.o
  HOSTCC  scripts/kconfig/parser.tab.o
  HOSTCC  scripts/kconfig/preprocess.o
  HOSTCC  scripts/kconfig/symbol.o
  HOSTCC  scripts/kconfig/util.o
  HOSTLD  scripts/kconfig/conf
#
# configuration written to .config
#
a0000@mcalab:~/raspberrypi-linux $ make ARCH=arm64 dtbs
  HOSTCC  scripts/dtc/dtc.o
  HOSTCC  scripts/dtc/flattree.o
  HOSTCC  scripts/dtc/fstree.o
  HOSTCC  scripts/dtc/data.o
  HOSTCC  scripts/dtc/livetree.o
  HOSTCC  scripts/dtc/treesource.o
  HOSTCC  scripts/dtc/srcpos.o
  HOSTCC  scripts/dtc/checks.o
  HOSTCC  scripts/dtc/util.o
  LEX     scripts/dtc/dtc-lexer.lex.c
  YACC    scripts/dtc/dtc-parser.tab.[ch]
  HOSTCC  scripts/dtc/dtc-lexer.lex.o
  HOSTCC  scripts/dtc/dtc-parser.tab.o
  HOSTLD  scripts/dtc/dtc
  HOSTCC  scripts/dtc/libfdt/fdt.o
  HOSTCC  scripts/dtc/libfdt/fdt_ro.o
  HOSTCC  scripts/dtc/libfdt/fdt_wip.o
  HOSTCC  scripts/dtc/libfdt/fdt_sw.o
  HOSTCC  scripts/dtc/libfdt/fdt_rw.o
  HOSTCC  scripts/dtc/libfdt/fdt_strerror.o
  HOSTCC  scripts/dtc/libfdt/fdt_empty_tree.o
  HOSTCC  scripts/dtc/libfdt/fdt_addresses.o
  HOSTCC  scripts/dtc/libfdt/fdt_overlay.o
  HOSTCC  scripts/dtc/fdtoverlay.o
  HOSTLD  scripts/dtc/fdtoverlay
  UPD     include/config/kernel.release
  DTC     arch/arm64/boot/dts/broadcom/bcm2711-rpi-400.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2711-rpi-4-b.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2711-rpi-cm4-io.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2712-rpi-5-b.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2712-d-rpi-5-b.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2837-rpi-3-a-plus.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2837-rpi-3-b.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2837-rpi-3-b-plus.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2837-rpi-cm3-io3.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2837-rpi-zero-2-w.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2710-rpi-zero-2.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2710-rpi-zero-2-w.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2710-rpi-2-b.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2710-rpi-3-b.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2710-rpi-cm0.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2710-rpi-cm3.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2711-rpi-cm4.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2711-rpi-cm4s.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2712d0-rpi-5-b.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2712-rpi-500.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2712-rpi-cm5-cm5io.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2712-rpi-cm5-cm4io.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2712-rpi-cm5l-cm5io.dtb
  DTC     arch/arm64/boot/dts/broadcom/bcm2712-rpi-cm5l-cm4io.dtb
  DTCO    arch/arm64/boot/dts/overlays/act-led.dtbo
  DTCO    arch/arm64/boot/dts/overlays/adafruit-st7735r.dtbo
  DTCO    arch/arm64/boot/dts/overlays/adafruit18.dtbo
  DTCO    arch/arm64/boot/dts/overlays/adau1977-adc.dtbo
  DTCO    arch/arm64/boot/dts/overlays/adau7002-simple.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ads1015.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ads1115.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ads7846.dtbo
  DTCO    arch/arm64/boot/dts/overlays/adv7282m.dtbo
  DTCO    arch/arm64/boot/dts/overlays/adv728x-m.dtbo
  DTCO    arch/arm64/boot/dts/overlays/akkordion-iqdacplus.dtbo
  DTCO    arch/arm64/boot/dts/overlays/allo-boss-dac-pcm512x-audio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/allo-boss2-dac-audio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/allo-digione.dtbo
  DTCO    arch/arm64/boot/dts/overlays/allo-katana-dac-audio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/allo-piano-dac-pcm512x-audio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/allo-piano-dac-plus-pcm512x-audio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/anyspi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/apds9960.dtbo
  DTCO    arch/arm64/boot/dts/overlays/applepi-dac.dtbo
  DTCO    arch/arm64/boot/dts/overlays/arducam-64mp.dtbo
  DTCO    arch/arm64/boot/dts/overlays/arducam-pivariety.dtbo
  DTCO    arch/arm64/boot/dts/overlays/at86rf233.dtbo
  DTCO    arch/arm64/boot/dts/overlays/audioinjector-addons.dtbo
  DTCO    arch/arm64/boot/dts/overlays/audioinjector-bare-i2s.dtbo
  DTCO    arch/arm64/boot/dts/overlays/audioinjector-isolated-soundcard.dtbo
  DTCO    arch/arm64/boot/dts/overlays/audioinjector-ultra.dtbo
  DTCO    arch/arm64/boot/dts/overlays/audioinjector-wm8731-audio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/audiosense-pi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/audremap.dtbo
  DTCO    arch/arm64/boot/dts/overlays/audremap-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/balena-fin.dtbo
  DTCO    arch/arm64/boot/dts/overlays/bcm2712d0.dtbo
  DTCO    arch/arm64/boot/dts/overlays/camera-mux-2port.dtbo
  DTCO    arch/arm64/boot/dts/overlays/camera-mux-4port.dtbo
  DTCO    arch/arm64/boot/dts/overlays/cap1106.dtbo
  DTCO    arch/arm64/boot/dts/overlays/chipcap2.dtbo
  DTCO    arch/arm64/boot/dts/overlays/chipdip-dac.dtbo
  DTCO    arch/arm64/boot/dts/overlays/cirrus-wm5102.dtbo
  DTCO    arch/arm64/boot/dts/overlays/cm-swap-i2c0.dtbo
  DTCO    arch/arm64/boot/dts/overlays/cma.dtbo
  DTCO    arch/arm64/boot/dts/overlays/crystalfontz-cfa050_pi_m.dtbo
  DTCO    arch/arm64/boot/dts/overlays/cutiepi-panel.dtbo
  DTCO    arch/arm64/boot/dts/overlays/dacberry400.dtbo
  DTCO    arch/arm64/boot/dts/overlays/dht11.dtbo
  DTCO    arch/arm64/boot/dts/overlays/dionaudio-kiwi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/dionaudio-loco.dtbo
  DTCO    arch/arm64/boot/dts/overlays/dionaudio-loco-v2.dtbo
  DTCO    arch/arm64/boot/dts/overlays/disable-bt.dtbo
  DTCO    arch/arm64/boot/dts/overlays/disable-bt-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/disable-emmc2.dtbo
  DTCO    arch/arm64/boot/dts/overlays/disable-wifi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/disable-wifi-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/dpi18.dtbo
  DTCO    arch/arm64/boot/dts/overlays/dpi18cpadhi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/dpi24.dtbo
  DTCO    arch/arm64/boot/dts/overlays/draws.dtbo
  DTCO    arch/arm64/boot/dts/overlays/dwc-otg-deprecated.dtbo
  DTCO    arch/arm64/boot/dts/overlays/dwc2.dtbo
  DTCO    arch/arm64/boot/dts/overlays/edt-ft5406.dtbo
  DTCO    arch/arm64/boot/dts/overlays/enc28j60.dtbo
  DTCO    arch/arm64/boot/dts/overlays/enc28j60-spi2.dtbo
  DTCO    arch/arm64/boot/dts/overlays/exc3000.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ezsound-6x8iso.dtbo
  DTCO    arch/arm64/boot/dts/overlays/fbtft.dtbo
  DTCO    arch/arm64/boot/dts/overlays/fe-pi-audio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/fsm-demo.dtbo
  DTCO    arch/arm64/boot/dts/overlays/gc9a01.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ghost-amp.dtbo
  DTCO    arch/arm64/boot/dts/overlays/goodix.dtbo
  DTCO    arch/arm64/boot/dts/overlays/googlevoicehat-soundcard.dtbo
  DTCO    arch/arm64/boot/dts/overlays/gpio-charger.dtbo
  DTCO    arch/arm64/boot/dts/overlays/gpio-fan.dtbo
  DTCO    arch/arm64/boot/dts/overlays/gpio-hog.dtbo
  DTCO    arch/arm64/boot/dts/overlays/gpio-ir.dtbo
  DTCO    arch/arm64/boot/dts/overlays/gpio-ir-tx.dtbo
  DTCO    arch/arm64/boot/dts/overlays/gpio-key.dtbo
  DTCO    arch/arm64/boot/dts/overlays/gpio-led.dtbo
  DTCO    arch/arm64/boot/dts/overlays/gpio-no-bank0-irq.dtbo
  DTCO    arch/arm64/boot/dts/overlays/gpio-no-irq.dtbo
  DTCO    arch/arm64/boot/dts/overlays/gpio-poweroff.dtbo
  DTCO    arch/arm64/boot/dts/overlays/gpio-shutdown.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hd44780-i2c-lcd.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hd44780-lcd.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hdmi-backlight-hwhack-gpio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hifiberry-adc.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hifiberry-adc8x.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hifiberry-amp.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hifiberry-amp100.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hifiberry-amp3.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hifiberry-amp4pro.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hifiberry-dac.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hifiberry-dac8x.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hifiberry-dacplus.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hifiberry-dacplus-pro.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hifiberry-dacplus-std.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hifiberry-dacplusadc.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hifiberry-dacplusadcpro.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hifiberry-dacplusdsp.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hifiberry-dacplushd.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hifiberry-digi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hifiberry-digi-pro.dtbo
  DTCO    arch/arm64/boot/dts/overlays/highperi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hy28a.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hy28b.dtbo
  DTCO    arch/arm64/boot/dts/overlays/hy28b-2017.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i-sabre-q2m.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c-bcm2708.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c-fan.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c-gpio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c-mux.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c-pwm-pca9685a.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c-rtc.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c-rtc-gpio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c-sensor.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c0.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c0-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c1.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c1-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c2-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c3.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c3-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c4.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2c6.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2s-dac.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2s-gpio28-31.dtbo
  DTCO    arch/arm64/boot/dts/overlays/i2s-master-dac.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ilitek251x.dtbo
  DTCO    arch/arm64/boot/dts/overlays/imx219.dtbo
  DTCO    arch/arm64/boot/dts/overlays/imx258.dtbo
  DTCO    arch/arm64/boot/dts/overlays/imx283.dtbo
  DTCO    arch/arm64/boot/dts/overlays/imx290.dtbo
  DTCO    arch/arm64/boot/dts/overlays/imx296.dtbo
  DTCO    arch/arm64/boot/dts/overlays/imx327.dtbo
  DTCO    arch/arm64/boot/dts/overlays/imx335.dtbo
  DTCO    arch/arm64/boot/dts/overlays/imx378.dtbo
  DTCO    arch/arm64/boot/dts/overlays/imx415.dtbo
  DTCO    arch/arm64/boot/dts/overlays/imx462.dtbo
  DTCO    arch/arm64/boot/dts/overlays/imx477.dtbo
  DTCO    arch/arm64/boot/dts/overlays/imx500.dtbo
  DTCO    arch/arm64/boot/dts/overlays/imx500-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/imx519.dtbo
  DTCO    arch/arm64/boot/dts/overlays/imx708.dtbo
  DTCO    arch/arm64/boot/dts/overlays/interludeaudio-analog.dtbo
  DTCO    arch/arm64/boot/dts/overlays/interludeaudio-digital.dtbo
  DTCO    arch/arm64/boot/dts/overlays/iqaudio-codec.dtbo
  DTCO    arch/arm64/boot/dts/overlays/iqaudio-dac.dtbo
  DTCO    arch/arm64/boot/dts/overlays/iqaudio-dacplus.dtbo
  DTCO    arch/arm64/boot/dts/overlays/iqaudio-digi-wm8804-audio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/iqs550.dtbo
  DTCO    arch/arm64/boot/dts/overlays/irs1125.dtbo
  DTCO    arch/arm64/boot/dts/overlays/jedec-spi-nor.dtbo
  DTCO    arch/arm64/boot/dts/overlays/justboom-both.dtbo
  DTCO    arch/arm64/boot/dts/overlays/justboom-dac.dtbo
  DTCO    arch/arm64/boot/dts/overlays/justboom-digi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ltc294x.dtbo
  DTCO    arch/arm64/boot/dts/overlays/max98357a.dtbo
  DTCO    arch/arm64/boot/dts/overlays/maxtherm.dtbo
  DTCO    arch/arm64/boot/dts/overlays/mbed-dac.dtbo
  DTCO    arch/arm64/boot/dts/overlays/mcp23017.dtbo
  DTCO    arch/arm64/boot/dts/overlays/mcp23s17.dtbo
  DTCO    arch/arm64/boot/dts/overlays/mcp2515.dtbo
  DTCO    arch/arm64/boot/dts/overlays/mcp2515-can0.dtbo
  DTCO    arch/arm64/boot/dts/overlays/mcp2515-can1.dtbo
  DTCO    arch/arm64/boot/dts/overlays/mcp251xfd.dtbo
  DTCO    arch/arm64/boot/dts/overlays/mcp3008.dtbo
  DTCO    arch/arm64/boot/dts/overlays/mcp3202.dtbo
  DTCO    arch/arm64/boot/dts/overlays/mcp342x.dtbo
  DTCO    arch/arm64/boot/dts/overlays/media-center.dtbo
  DTCO    arch/arm64/boot/dts/overlays/merus-amp.dtbo
  DTCO    arch/arm64/boot/dts/overlays/midi-uart0.dtbo
  DTCO    arch/arm64/boot/dts/overlays/midi-uart0-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/midi-uart1.dtbo
  DTCO    arch/arm64/boot/dts/overlays/midi-uart1-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/midi-uart2.dtbo
  DTCO    arch/arm64/boot/dts/overlays/midi-uart2-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/midi-uart3.dtbo
  DTCO    arch/arm64/boot/dts/overlays/midi-uart3-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/midi-uart4.dtbo
  DTCO    arch/arm64/boot/dts/overlays/midi-uart4-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/midi-uart5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/minipitft13.dtbo
  DTCO    arch/arm64/boot/dts/overlays/miniuart-bt.dtbo
  DTCO    arch/arm64/boot/dts/overlays/mipi-dbi-spi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/mira220.dtbo
  DTCO    arch/arm64/boot/dts/overlays/mlx90640.dtbo
  DTCO    arch/arm64/boot/dts/overlays/mmc.dtbo
  DTCO    arch/arm64/boot/dts/overlays/mz61581.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ov2311.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ov5647.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ov64a40.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ov7251.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ov9281.dtbo
  DTCO    arch/arm64/boot/dts/overlays/papirus.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pca953x.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pcf857x.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pcie-32bit-dma.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pcie-32bit-dma-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pciex1-compat-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pibell.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pifacedigital.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pifi-40.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pifi-dac-hd.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pifi-dac-zero.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pifi-mini-210.dtbo
  DTCO    arch/arm64/boot/dts/overlays/piglow.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pimidi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pineboards-hat-ai.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pineboards-hatdrive-poe-plus.dtbo
  DTCO    arch/arm64/boot/dts/overlays/piscreen.dtbo
  DTCO    arch/arm64/boot/dts/overlays/piscreen2r.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pisound.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pisound-micro.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pisound-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pitft22.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pitft28-capacitive.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pitft28-resistive.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pitft35-resistive.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pivision.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pps-gpio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/proto-codec.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pwm.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pwm-2chan.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pwm-gpio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pwm-gpio-fan.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pwm-ir-tx.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pwm-pio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/pwm1.dtbo
  DTCO    arch/arm64/boot/dts/overlays/qca7000.dtbo
  DTCO    arch/arm64/boot/dts/overlays/qca7000-uart0.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ramoops.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ramoops-pi4.dtbo
  DTCO    arch/arm64/boot/dts/overlays/rootmaster.dtbo
  DTCO    arch/arm64/boot/dts/overlays/rotary-encoder.dtbo
  DTCO    arch/arm64/boot/dts/overlays/rpi-backlight.dtbo
  DTCO    arch/arm64/boot/dts/overlays/rpi-codeczero.dtbo
  DTCO    arch/arm64/boot/dts/overlays/rpi-dacplus.dtbo
  DTCO    arch/arm64/boot/dts/overlays/rpi-dacpro.dtbo
  DTCO    arch/arm64/boot/dts/overlays/rpi-digiampplus.dtbo
  DTCO    arch/arm64/boot/dts/overlays/rpi-ft5406.dtbo
  DTCO    arch/arm64/boot/dts/overlays/rpi-fw-uart.dtbo
  DTCO    arch/arm64/boot/dts/overlays/rpi-poe.dtbo
  DTCO    arch/arm64/boot/dts/overlays/rpi-poe-plus.dtbo
  DTCO    arch/arm64/boot/dts/overlays/rpi-sense.dtbo
  DTCO    arch/arm64/boot/dts/overlays/rpi-sense-v2.dtbo
  DTCO    arch/arm64/boot/dts/overlays/rpi-tv.dtbo
  DTCO    arch/arm64/boot/dts/overlays/rra-digidac1-wm8741-audio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/sainsmart18.dtbo
  DTCO    arch/arm64/boot/dts/overlays/sc16is750-i2c.dtbo
  DTCO    arch/arm64/boot/dts/overlays/sc16is752-i2c.dtbo
  DTCO    arch/arm64/boot/dts/overlays/sc16is75x-spi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/sdhost.dtbo
  DTCO    arch/arm64/boot/dts/overlays/sdio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/sdio-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/seeed-can-fd-hat-v1.dtbo
  DTCO    arch/arm64/boot/dts/overlays/seeed-can-fd-hat-v2.dtbo
  DTCO    arch/arm64/boot/dts/overlays/sh1106-spi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/si446x-spi0.dtbo
  DTCO    arch/arm64/boot/dts/overlays/smi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/smi-dev.dtbo
  DTCO    arch/arm64/boot/dts/overlays/smi-nand.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi-gpio35-39.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi-gpio40-45.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi-rtc.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi0-0cs.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi0-1cs.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi0-1cs-inverted.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi0-2cs.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi1-1cs.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi1-2cs.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi1-3cs.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi2-1cs.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi2-1cs-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi2-2cs.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi2-2cs-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi2-3cs.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi3-1cs.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi3-1cs-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi3-2cs.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi3-2cs-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi4-1cs.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi4-2cs.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi5-1cs.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi5-1cs-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi5-2cs.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi5-2cs-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi6-1cs.dtbo
  DTCO    arch/arm64/boot/dts/overlays/spi6-2cs.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ssd1306.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ssd1306-spi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ssd1327-spi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ssd1331-spi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ssd1351-spi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/sunfounder-pipower3.dtbo
  DTCO    arch/arm64/boot/dts/overlays/sunfounder-pironman5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/superaudioboard.dtbo
  DTCO    arch/arm64/boot/dts/overlays/sx150x.dtbo
  DTCO    arch/arm64/boot/dts/overlays/tc358743.dtbo
  DTCO    arch/arm64/boot/dts/overlays/tc358743-audio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/tc358743-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/tinylcd35.dtbo
  DTCO    arch/arm64/boot/dts/overlays/tpm-slb9670.dtbo
  DTCO    arch/arm64/boot/dts/overlays/tpm-slb9673.dtbo
  DTCO    arch/arm64/boot/dts/overlays/uart0.dtbo
  DTCO    arch/arm64/boot/dts/overlays/uart0-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/uart1.dtbo
  DTCO    arch/arm64/boot/dts/overlays/uart1-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/uart2.dtbo
  DTCO    arch/arm64/boot/dts/overlays/uart2-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/uart3.dtbo
  DTCO    arch/arm64/boot/dts/overlays/uart3-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/uart4.dtbo
  DTCO    arch/arm64/boot/dts/overlays/uart4-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/uart5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/udrc.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ugreen-dabboard.dtbo
  DTCO    arch/arm64/boot/dts/overlays/upstream.dtbo
  DTCO    arch/arm64/boot/dts/overlays/upstream-pi4.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-fkms-v3d.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-fkms-v3d-pi4.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-dpi-generic.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-dpi-hyperpixel2r.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-dpi-hyperpixel4.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-dpi-hyperpixel4sq.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-dpi-panel.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-dsi-7inch.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-dsi-generic.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-dsi-ili9881-5inch.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-dsi-ili9881-7inch.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-dsi-lt070me05000.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-dsi-lt070me05000-v2.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-dsi-waveshare-800x480.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-dsi-waveshare-panel.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-dsi-waveshare-panel-v2.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-kippah-7inch.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-v3d.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-v3d-pi4.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-v3d-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vc4-kms-vga666.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vga666.dtbo
  DTCO    arch/arm64/boot/dts/overlays/vl805.dtbo
  DTCO    arch/arm64/boot/dts/overlays/w1-gpio.dtbo
  DTCO    arch/arm64/boot/dts/overlays/w1-gpio-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/w1-gpio-pullup.dtbo
  DTCO    arch/arm64/boot/dts/overlays/w1-gpio-pullup-pi5.dtbo
  DTCO    arch/arm64/boot/dts/overlays/w5500.dtbo
  DTCO    arch/arm64/boot/dts/overlays/watterott-display.dtbo
  DTCO    arch/arm64/boot/dts/overlays/waveshare-can-fd-hat-mode-a.dtbo
  DTCO    arch/arm64/boot/dts/overlays/waveshare-can-fd-hat-mode-b.dtbo
  DTCO    arch/arm64/boot/dts/overlays/wifimac.dtbo
  DTCO    arch/arm64/boot/dts/overlays/wittypi.dtbo
  DTCO    arch/arm64/boot/dts/overlays/wm8960-soundcard.dtbo
  DTCO    arch/arm64/boot/dts/overlays/ws2812-pio.dtbo
  DTC     arch/arm64/boot/dts/overlays/overlay_map.dtb
  DTC     arch/arm64/boot/dts/overlays/hat_map.dtb
a0000@mcalab:~/raspberrypi-linux $ ls
arch   certs    CREDITS  Documentation  fs       init      ipc     Kconfig  lib       MAINTAINERS  mm   README     rust     scripts   sound  usr
block  COPYING  crypto   drivers        include  io_uring  Kbuild  kernel   LICENSES  Makefile     net  README.md  samples  security  tools  virt
a0000@mcalab:~/raspberrypi-linux $ ls arch/arm64/boot/dts/broadcom/*.dtb
arch/arm64/boot/dts/broadcom/bcm2710-rpi-2-b.dtb       arch/arm64/boot/dts/broadcom/bcm2711-rpi-400.dtb     arch/arm64/boot/dts/broadcom/bcm2712-rpi-500.dtb         arch/arm64/boot/dts/broadcom/bcm2837-rpi-3-b.dtb
arch/arm64/boot/dts/broadcom/bcm2710-rpi-3-b.dtb       arch/arm64/boot/dts/broadcom/bcm2711-rpi-4-b.dtb     arch/arm64/boot/dts/broadcom/bcm2712-rpi-5-b.dtb         arch/arm64/boot/dts/broadcom/bcm2837-rpi-3-b-plus.dtb
arch/arm64/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dtb  arch/arm64/boot/dts/broadcom/bcm2711-rpi-cm4.dtb     arch/arm64/boot/dts/broadcom/bcm2712-rpi-cm5-cm4io.dtb   arch/arm64/boot/dts/broadcom/bcm2837-rpi-cm3-io3.dtb
arch/arm64/boot/dts/broadcom/bcm2710-rpi-cm0.dtb       arch/arm64/boot/dts/broadcom/bcm2711-rpi-cm4-io.dtb  arch/arm64/boot/dts/broadcom/bcm2712-rpi-cm5-cm5io.dtb   arch/arm64/boot/dts/broadcom/bcm2837-rpi-zero-2-w.dtb
arch/arm64/boot/dts/broadcom/bcm2710-rpi-cm3.dtb       arch/arm64/boot/dts/broadcom/bcm2711-rpi-cm4s.dtb    arch/arm64/boot/dts/broadcom/bcm2712-rpi-cm5l-cm4io.dtb
arch/arm64/boot/dts/broadcom/bcm2710-rpi-zero-2.dtb    arch/arm64/boot/dts/broadcom/bcm2712d0-rpi-5-b.dtb   arch/arm64/boot/dts/broadcom/bcm2712-rpi-cm5l-cm5io.dtb
arch/arm64/boot/dts/broadcom/bcm2710-rpi-zero-2-w.dtb  arch/arm64/boot/dts/broadcom/bcm2712-d-rpi-5-b.dtb   arch/arm64/boot/dts/broadcom/bcm2837-rpi-3-a-plus.dtb
a0000@mcalab:~/raspberrypi-linux $ ls arch/arm64/boot/dts/broadcom/*.dtbo
ls: cannot access 'arch/arm64/boot/dts/broadcom/*.dtbo': No such file or directory
a0000@mcalab:~/raspberrypi-linux $ ls arch/arm64/boot/dts/overlays/*.dtbo
arch/arm64/boot/dts/overlays/act-led.dtbo                            arch/arm64/boot/dts/overlays/i2c-gpio.dtbo                      arch/arm64/boot/dts/overlays/rpi-codeczero.dtbo
arch/arm64/boot/dts/overlays/adafruit18.dtbo                         arch/arm64/boot/dts/overlays/i2c-mux.dtbo                       arch/arm64/boot/dts/overlays/rpi-dacplus.dtbo
arch/arm64/boot/dts/overlays/adafruit-st7735r.dtbo                   arch/arm64/boot/dts/overlays/i2c-pwm-pca9685a.dtbo              arch/arm64/boot/dts/overlays/rpi-dacpro.dtbo
arch/arm64/boot/dts/overlays/adau1977-adc.dtbo                       arch/arm64/boot/dts/overlays/i2c-rtc.dtbo                       arch/arm64/boot/dts/overlays/rpi-digiampplus.dtbo
arch/arm64/boot/dts/overlays/adau7002-simple.dtbo                    arch/arm64/boot/dts/overlays/i2c-rtc-gpio.dtbo                  arch/arm64/boot/dts/overlays/rpi-ft5406.dtbo
arch/arm64/boot/dts/overlays/ads1015.dtbo                            arch/arm64/boot/dts/overlays/i2c-sensor.dtbo                    arch/arm64/boot/dts/overlays/rpi-fw-uart.dtbo
arch/arm64/boot/dts/overlays/ads1115.dtbo                            arch/arm64/boot/dts/overlays/i2s-dac.dtbo                       arch/arm64/boot/dts/overlays/rpi-poe.dtbo
arch/arm64/boot/dts/overlays/ads7846.dtbo                            arch/arm64/boot/dts/overlays/i2s-gpio28-31.dtbo                 arch/arm64/boot/dts/overlays/rpi-poe-plus.dtbo
arch/arm64/boot/dts/overlays/adv7282m.dtbo                           arch/arm64/boot/dts/overlays/i2s-master-dac.dtbo                arch/arm64/boot/dts/overlays/rpi-sense.dtbo
arch/arm64/boot/dts/overlays/adv728x-m.dtbo                          arch/arm64/boot/dts/overlays/ilitek251x.dtbo                    arch/arm64/boot/dts/overlays/rpi-sense-v2.dtbo
arch/arm64/boot/dts/overlays/akkordion-iqdacplus.dtbo                arch/arm64/boot/dts/overlays/imx219.dtbo                        arch/arm64/boot/dts/overlays/rpi-tv.dtbo
arch/arm64/boot/dts/overlays/allo-boss2-dac-audio.dtbo               arch/arm64/boot/dts/overlays/imx258.dtbo                        arch/arm64/boot/dts/overlays/rra-digidac1-wm8741-audio.dtbo
arch/arm64/boot/dts/overlays/allo-boss-dac-pcm512x-audio.dtbo        arch/arm64/boot/dts/overlays/imx283.dtbo                        arch/arm64/boot/dts/overlays/sainsmart18.dtbo
arch/arm64/boot/dts/overlays/allo-digione.dtbo                       arch/arm64/boot/dts/overlays/imx290.dtbo                        arch/arm64/boot/dts/overlays/sc16is750-i2c.dtbo
arch/arm64/boot/dts/overlays/allo-katana-dac-audio.dtbo              arch/arm64/boot/dts/overlays/imx296.dtbo                        arch/arm64/boot/dts/overlays/sc16is752-i2c.dtbo
arch/arm64/boot/dts/overlays/allo-piano-dac-pcm512x-audio.dtbo       arch/arm64/boot/dts/overlays/imx327.dtbo                        arch/arm64/boot/dts/overlays/sc16is75x-spi.dtbo
arch/arm64/boot/dts/overlays/allo-piano-dac-plus-pcm512x-audio.dtbo  arch/arm64/boot/dts/overlays/imx335.dtbo                        arch/arm64/boot/dts/overlays/sdhost.dtbo
arch/arm64/boot/dts/overlays/anyspi.dtbo                             arch/arm64/boot/dts/overlays/imx378.dtbo                        arch/arm64/boot/dts/overlays/sdio.dtbo
arch/arm64/boot/dts/overlays/apds9960.dtbo                           arch/arm64/boot/dts/overlays/imx415.dtbo                        arch/arm64/boot/dts/overlays/sdio-pi5.dtbo
arch/arm64/boot/dts/overlays/applepi-dac.dtbo                        arch/arm64/boot/dts/overlays/imx462.dtbo                        arch/arm64/boot/dts/overlays/seeed-can-fd-hat-v1.dtbo
arch/arm64/boot/dts/overlays/arducam-64mp.dtbo                       arch/arm64/boot/dts/overlays/imx477.dtbo                        arch/arm64/boot/dts/overlays/seeed-can-fd-hat-v2.dtbo
arch/arm64/boot/dts/overlays/arducam-pivariety.dtbo                  arch/arm64/boot/dts/overlays/imx500.dtbo                        arch/arm64/boot/dts/overlays/sh1106-spi.dtbo
arch/arm64/boot/dts/overlays/at86rf233.dtbo                          arch/arm64/boot/dts/overlays/imx500-pi5.dtbo                    arch/arm64/boot/dts/overlays/si446x-spi0.dtbo
arch/arm64/boot/dts/overlays/audioinjector-addons.dtbo               arch/arm64/boot/dts/overlays/imx519.dtbo                        arch/arm64/boot/dts/overlays/smi-dev.dtbo
arch/arm64/boot/dts/overlays/audioinjector-bare-i2s.dtbo             arch/arm64/boot/dts/overlays/imx708.dtbo                        arch/arm64/boot/dts/overlays/smi.dtbo
arch/arm64/boot/dts/overlays/audioinjector-isolated-soundcard.dtbo   arch/arm64/boot/dts/overlays/interludeaudio-analog.dtbo         arch/arm64/boot/dts/overlays/smi-nand.dtbo
arch/arm64/boot/dts/overlays/audioinjector-ultra.dtbo                arch/arm64/boot/dts/overlays/interludeaudio-digital.dtbo        arch/arm64/boot/dts/overlays/spi0-0cs.dtbo
arch/arm64/boot/dts/overlays/audioinjector-wm8731-audio.dtbo         arch/arm64/boot/dts/overlays/iqaudio-codec.dtbo                 arch/arm64/boot/dts/overlays/spi0-1cs.dtbo
arch/arm64/boot/dts/overlays/audiosense-pi.dtbo                      arch/arm64/boot/dts/overlays/iqaudio-dac.dtbo                   arch/arm64/boot/dts/overlays/spi0-1cs-inverted.dtbo
arch/arm64/boot/dts/overlays/audremap.dtbo                           arch/arm64/boot/dts/overlays/iqaudio-dacplus.dtbo               arch/arm64/boot/dts/overlays/spi0-2cs.dtbo
arch/arm64/boot/dts/overlays/audremap-pi5.dtbo                       arch/arm64/boot/dts/overlays/iqaudio-digi-wm8804-audio.dtbo     arch/arm64/boot/dts/overlays/spi1-1cs.dtbo
arch/arm64/boot/dts/overlays/balena-fin.dtbo                         arch/arm64/boot/dts/overlays/iqs550.dtbo                        arch/arm64/boot/dts/overlays/spi1-2cs.dtbo
arch/arm64/boot/dts/overlays/bcm2712d0.dtbo                          arch/arm64/boot/dts/overlays/irs1125.dtbo                       arch/arm64/boot/dts/overlays/spi1-3cs.dtbo
arch/arm64/boot/dts/overlays/camera-mux-2port.dtbo                   arch/arm64/boot/dts/overlays/i-sabre-q2m.dtbo                   arch/arm64/boot/dts/overlays/spi2-1cs.dtbo
arch/arm64/boot/dts/overlays/camera-mux-4port.dtbo                   arch/arm64/boot/dts/overlays/jedec-spi-nor.dtbo                 arch/arm64/boot/dts/overlays/spi2-1cs-pi5.dtbo
arch/arm64/boot/dts/overlays/cap1106.dtbo                            arch/arm64/boot/dts/overlays/justboom-both.dtbo                 arch/arm64/boot/dts/overlays/spi2-2cs.dtbo
arch/arm64/boot/dts/overlays/chipcap2.dtbo                           arch/arm64/boot/dts/overlays/justboom-dac.dtbo                  arch/arm64/boot/dts/overlays/spi2-2cs-pi5.dtbo
arch/arm64/boot/dts/overlays/chipdip-dac.dtbo                        arch/arm64/boot/dts/overlays/justboom-digi.dtbo                 arch/arm64/boot/dts/overlays/spi2-3cs.dtbo
arch/arm64/boot/dts/overlays/cirrus-wm5102.dtbo                      arch/arm64/boot/dts/overlays/ltc294x.dtbo                       arch/arm64/boot/dts/overlays/spi3-1cs.dtbo
arch/arm64/boot/dts/overlays/cma.dtbo                                arch/arm64/boot/dts/overlays/max98357a.dtbo                     arch/arm64/boot/dts/overlays/spi3-1cs-pi5.dtbo
arch/arm64/boot/dts/overlays/cm-swap-i2c0.dtbo                       arch/arm64/boot/dts/overlays/maxtherm.dtbo                      arch/arm64/boot/dts/overlays/spi3-2cs.dtbo
arch/arm64/boot/dts/overlays/crystalfontz-cfa050_pi_m.dtbo           arch/arm64/boot/dts/overlays/mbed-dac.dtbo                      arch/arm64/boot/dts/overlays/spi3-2cs-pi5.dtbo
arch/arm64/boot/dts/overlays/cutiepi-panel.dtbo                      arch/arm64/boot/dts/overlays/mcp23017.dtbo                      arch/arm64/boot/dts/overlays/spi4-1cs.dtbo
arch/arm64/boot/dts/overlays/dacberry400.dtbo                        arch/arm64/boot/dts/overlays/mcp23s17.dtbo                      arch/arm64/boot/dts/overlays/spi4-2cs.dtbo
arch/arm64/boot/dts/overlays/dht11.dtbo                              arch/arm64/boot/dts/overlays/mcp2515-can0.dtbo                  arch/arm64/boot/dts/overlays/spi5-1cs.dtbo
arch/arm64/boot/dts/overlays/dionaudio-kiwi.dtbo                     arch/arm64/boot/dts/overlays/mcp2515-can1.dtbo                  arch/arm64/boot/dts/overlays/spi5-1cs-pi5.dtbo
arch/arm64/boot/dts/overlays/dionaudio-loco.dtbo                     arch/arm64/boot/dts/overlays/mcp2515.dtbo                       arch/arm64/boot/dts/overlays/spi5-2cs.dtbo
arch/arm64/boot/dts/overlays/dionaudio-loco-v2.dtbo                  arch/arm64/boot/dts/overlays/mcp251xfd.dtbo                     arch/arm64/boot/dts/overlays/spi5-2cs-pi5.dtbo
arch/arm64/boot/dts/overlays/disable-bt.dtbo                         arch/arm64/boot/dts/overlays/mcp3008.dtbo                       arch/arm64/boot/dts/overlays/spi6-1cs.dtbo
arch/arm64/boot/dts/overlays/disable-bt-pi5.dtbo                     arch/arm64/boot/dts/overlays/mcp3202.dtbo                       arch/arm64/boot/dts/overlays/spi6-2cs.dtbo
arch/arm64/boot/dts/overlays/disable-emmc2.dtbo                      arch/arm64/boot/dts/overlays/mcp342x.dtbo                       arch/arm64/boot/dts/overlays/spi-gpio35-39.dtbo
arch/arm64/boot/dts/overlays/disable-wifi.dtbo                       arch/arm64/boot/dts/overlays/media-center.dtbo                  arch/arm64/boot/dts/overlays/spi-gpio40-45.dtbo
arch/arm64/boot/dts/overlays/disable-wifi-pi5.dtbo                   arch/arm64/boot/dts/overlays/merus-amp.dtbo                     arch/arm64/boot/dts/overlays/spi-rtc.dtbo
arch/arm64/boot/dts/overlays/dpi18cpadhi.dtbo                        arch/arm64/boot/dts/overlays/midi-uart0.dtbo                    arch/arm64/boot/dts/overlays/ssd1306.dtbo
arch/arm64/boot/dts/overlays/dpi18.dtbo                              arch/arm64/boot/dts/overlays/midi-uart0-pi5.dtbo                arch/arm64/boot/dts/overlays/ssd1306-spi.dtbo
arch/arm64/boot/dts/overlays/dpi24.dtbo                              arch/arm64/boot/dts/overlays/midi-uart1.dtbo                    arch/arm64/boot/dts/overlays/ssd1327-spi.dtbo
arch/arm64/boot/dts/overlays/draws.dtbo                              arch/arm64/boot/dts/overlays/midi-uart1-pi5.dtbo                arch/arm64/boot/dts/overlays/ssd1331-spi.dtbo
arch/arm64/boot/dts/overlays/dwc2.dtbo                               arch/arm64/boot/dts/overlays/midi-uart2.dtbo                    arch/arm64/boot/dts/overlays/ssd1351-spi.dtbo
arch/arm64/boot/dts/overlays/dwc-otg-deprecated.dtbo                 arch/arm64/boot/dts/overlays/midi-uart2-pi5.dtbo                arch/arm64/boot/dts/overlays/sunfounder-pipower3.dtbo
arch/arm64/boot/dts/overlays/edt-ft5406.dtbo                         arch/arm64/boot/dts/overlays/midi-uart3.dtbo                    arch/arm64/boot/dts/overlays/sunfounder-pironman5.dtbo
arch/arm64/boot/dts/overlays/enc28j60.dtbo                           arch/arm64/boot/dts/overlays/midi-uart3-pi5.dtbo                arch/arm64/boot/dts/overlays/superaudioboard.dtbo
arch/arm64/boot/dts/overlays/enc28j60-spi2.dtbo                      arch/arm64/boot/dts/overlays/midi-uart4.dtbo                    arch/arm64/boot/dts/overlays/sx150x.dtbo
arch/arm64/boot/dts/overlays/exc3000.dtbo                            arch/arm64/boot/dts/overlays/midi-uart4-pi5.dtbo                arch/arm64/boot/dts/overlays/tc358743-audio.dtbo
arch/arm64/boot/dts/overlays/ezsound-6x8iso.dtbo                     arch/arm64/boot/dts/overlays/midi-uart5.dtbo                    arch/arm64/boot/dts/overlays/tc358743.dtbo
arch/arm64/boot/dts/overlays/fbtft.dtbo                              arch/arm64/boot/dts/overlays/minipitft13.dtbo                   arch/arm64/boot/dts/overlays/tc358743-pi5.dtbo
arch/arm64/boot/dts/overlays/fe-pi-audio.dtbo                        arch/arm64/boot/dts/overlays/miniuart-bt.dtbo                   arch/arm64/boot/dts/overlays/tinylcd35.dtbo
arch/arm64/boot/dts/overlays/fsm-demo.dtbo                           arch/arm64/boot/dts/overlays/mipi-dbi-spi.dtbo                  arch/arm64/boot/dts/overlays/tpm-slb9670.dtbo
arch/arm64/boot/dts/overlays/gc9a01.dtbo                             arch/arm64/boot/dts/overlays/mira220.dtbo                       arch/arm64/boot/dts/overlays/tpm-slb9673.dtbo
arch/arm64/boot/dts/overlays/ghost-amp.dtbo                          arch/arm64/boot/dts/overlays/mlx90640.dtbo                      arch/arm64/boot/dts/overlays/uart0.dtbo
arch/arm64/boot/dts/overlays/goodix.dtbo                             arch/arm64/boot/dts/overlays/mmc.dtbo                           arch/arm64/boot/dts/overlays/uart0-pi5.dtbo
arch/arm64/boot/dts/overlays/googlevoicehat-soundcard.dtbo           arch/arm64/boot/dts/overlays/mz61581.dtbo                       arch/arm64/boot/dts/overlays/uart1.dtbo
arch/arm64/boot/dts/overlays/gpio-charger.dtbo                       arch/arm64/boot/dts/overlays/ov2311.dtbo                        arch/arm64/boot/dts/overlays/uart1-pi5.dtbo
arch/arm64/boot/dts/overlays/gpio-fan.dtbo                           arch/arm64/boot/dts/overlays/ov5647.dtbo                        arch/arm64/boot/dts/overlays/uart2.dtbo
arch/arm64/boot/dts/overlays/gpio-hog.dtbo                           arch/arm64/boot/dts/overlays/ov64a40.dtbo                       arch/arm64/boot/dts/overlays/uart2-pi5.dtbo
arch/arm64/boot/dts/overlays/gpio-ir.dtbo                            arch/arm64/boot/dts/overlays/ov7251.dtbo                        arch/arm64/boot/dts/overlays/uart3.dtbo
arch/arm64/boot/dts/overlays/gpio-ir-tx.dtbo                         arch/arm64/boot/dts/overlays/ov9281.dtbo                        arch/arm64/boot/dts/overlays/uart3-pi5.dtbo
arch/arm64/boot/dts/overlays/gpio-key.dtbo                           arch/arm64/boot/dts/overlays/papirus.dtbo                       arch/arm64/boot/dts/overlays/uart4.dtbo
arch/arm64/boot/dts/overlays/gpio-led.dtbo                           arch/arm64/boot/dts/overlays/pca953x.dtbo                       arch/arm64/boot/dts/overlays/uart4-pi5.dtbo
arch/arm64/boot/dts/overlays/gpio-no-bank0-irq.dtbo                  arch/arm64/boot/dts/overlays/pcf857x.dtbo                       arch/arm64/boot/dts/overlays/uart5.dtbo
arch/arm64/boot/dts/overlays/gpio-no-irq.dtbo                        arch/arm64/boot/dts/overlays/pcie-32bit-dma.dtbo                arch/arm64/boot/dts/overlays/udrc.dtbo
arch/arm64/boot/dts/overlays/gpio-poweroff.dtbo                      arch/arm64/boot/dts/overlays/pcie-32bit-dma-pi5.dtbo            arch/arm64/boot/dts/overlays/ugreen-dabboard.dtbo
arch/arm64/boot/dts/overlays/gpio-shutdown.dtbo                      arch/arm64/boot/dts/overlays/pciex1-compat-pi5.dtbo             arch/arm64/boot/dts/overlays/upstream.dtbo
arch/arm64/boot/dts/overlays/hd44780-i2c-lcd.dtbo                    arch/arm64/boot/dts/overlays/pibell.dtbo                        arch/arm64/boot/dts/overlays/upstream-pi4.dtbo
arch/arm64/boot/dts/overlays/hd44780-lcd.dtbo                        arch/arm64/boot/dts/overlays/pifacedigital.dtbo                 arch/arm64/boot/dts/overlays/vc4-fkms-v3d.dtbo
arch/arm64/boot/dts/overlays/hdmi-backlight-hwhack-gpio.dtbo         arch/arm64/boot/dts/overlays/pifi-40.dtbo                       arch/arm64/boot/dts/overlays/vc4-fkms-v3d-pi4.dtbo
arch/arm64/boot/dts/overlays/hifiberry-adc8x.dtbo                    arch/arm64/boot/dts/overlays/pifi-dac-hd.dtbo                   arch/arm64/boot/dts/overlays/vc4-kms-dpi-generic.dtbo
arch/arm64/boot/dts/overlays/hifiberry-adc.dtbo                      arch/arm64/boot/dts/overlays/pifi-dac-zero.dtbo                 arch/arm64/boot/dts/overlays/vc4-kms-dpi-hyperpixel2r.dtbo
arch/arm64/boot/dts/overlays/hifiberry-amp100.dtbo                   arch/arm64/boot/dts/overlays/pifi-mini-210.dtbo                 arch/arm64/boot/dts/overlays/vc4-kms-dpi-hyperpixel4.dtbo
arch/arm64/boot/dts/overlays/hifiberry-amp3.dtbo                     arch/arm64/boot/dts/overlays/piglow.dtbo                        arch/arm64/boot/dts/overlays/vc4-kms-dpi-hyperpixel4sq.dtbo
arch/arm64/boot/dts/overlays/hifiberry-amp4pro.dtbo                  arch/arm64/boot/dts/overlays/pimidi.dtbo                        arch/arm64/boot/dts/overlays/vc4-kms-dpi-panel.dtbo
arch/arm64/boot/dts/overlays/hifiberry-amp.dtbo                      arch/arm64/boot/dts/overlays/pineboards-hat-ai.dtbo             arch/arm64/boot/dts/overlays/vc4-kms-dsi-7inch.dtbo
arch/arm64/boot/dts/overlays/hifiberry-dac8x.dtbo                    arch/arm64/boot/dts/overlays/pineboards-hatdrive-poe-plus.dtbo  arch/arm64/boot/dts/overlays/vc4-kms-dsi-generic.dtbo
arch/arm64/boot/dts/overlays/hifiberry-dac.dtbo                      arch/arm64/boot/dts/overlays/piscreen2r.dtbo                    arch/arm64/boot/dts/overlays/vc4-kms-dsi-ili9881-5inch.dtbo
arch/arm64/boot/dts/overlays/hifiberry-dacplusadc.dtbo               arch/arm64/boot/dts/overlays/piscreen.dtbo                      arch/arm64/boot/dts/overlays/vc4-kms-dsi-ili9881-7inch.dtbo
arch/arm64/boot/dts/overlays/hifiberry-dacplusadcpro.dtbo            arch/arm64/boot/dts/overlays/pisound.dtbo                       arch/arm64/boot/dts/overlays/vc4-kms-dsi-lt070me05000.dtbo
arch/arm64/boot/dts/overlays/hifiberry-dacplusdsp.dtbo               arch/arm64/boot/dts/overlays/pisound-micro.dtbo                 arch/arm64/boot/dts/overlays/vc4-kms-dsi-lt070me05000-v2.dtbo
arch/arm64/boot/dts/overlays/hifiberry-dacplus.dtbo                  arch/arm64/boot/dts/overlays/pisound-pi5.dtbo                   arch/arm64/boot/dts/overlays/vc4-kms-dsi-waveshare-800x480.dtbo
arch/arm64/boot/dts/overlays/hifiberry-dacplushd.dtbo                arch/arm64/boot/dts/overlays/pitft22.dtbo                       arch/arm64/boot/dts/overlays/vc4-kms-dsi-waveshare-panel.dtbo
arch/arm64/boot/dts/overlays/hifiberry-dacplus-pro.dtbo              arch/arm64/boot/dts/overlays/pitft28-capacitive.dtbo            arch/arm64/boot/dts/overlays/vc4-kms-dsi-waveshare-panel-v2.dtbo
arch/arm64/boot/dts/overlays/hifiberry-dacplus-std.dtbo              arch/arm64/boot/dts/overlays/pitft28-resistive.dtbo             arch/arm64/boot/dts/overlays/vc4-kms-kippah-7inch.dtbo
arch/arm64/boot/dts/overlays/hifiberry-digi.dtbo                     arch/arm64/boot/dts/overlays/pitft35-resistive.dtbo             arch/arm64/boot/dts/overlays/vc4-kms-v3d.dtbo
arch/arm64/boot/dts/overlays/hifiberry-digi-pro.dtbo                 arch/arm64/boot/dts/overlays/pivision.dtbo                      arch/arm64/boot/dts/overlays/vc4-kms-v3d-pi4.dtbo
arch/arm64/boot/dts/overlays/highperi.dtbo                           arch/arm64/boot/dts/overlays/pps-gpio.dtbo                      arch/arm64/boot/dts/overlays/vc4-kms-v3d-pi5.dtbo
arch/arm64/boot/dts/overlays/hy28a.dtbo                              arch/arm64/boot/dts/overlays/proto-codec.dtbo                   arch/arm64/boot/dts/overlays/vc4-kms-vga666.dtbo
arch/arm64/boot/dts/overlays/hy28b-2017.dtbo                         arch/arm64/boot/dts/overlays/pwm1.dtbo                          arch/arm64/boot/dts/overlays/vga666.dtbo
arch/arm64/boot/dts/overlays/hy28b.dtbo                              arch/arm64/boot/dts/overlays/pwm-2chan.dtbo                     arch/arm64/boot/dts/overlays/vl805.dtbo
arch/arm64/boot/dts/overlays/i2c0.dtbo                               arch/arm64/boot/dts/overlays/pwm.dtbo                           arch/arm64/boot/dts/overlays/w1-gpio.dtbo
arch/arm64/boot/dts/overlays/i2c0-pi5.dtbo                           arch/arm64/boot/dts/overlays/pwm-gpio.dtbo                      arch/arm64/boot/dts/overlays/w1-gpio-pi5.dtbo
arch/arm64/boot/dts/overlays/i2c1.dtbo                               arch/arm64/boot/dts/overlays/pwm-gpio-fan.dtbo                  arch/arm64/boot/dts/overlays/w1-gpio-pullup.dtbo
arch/arm64/boot/dts/overlays/i2c1-pi5.dtbo                           arch/arm64/boot/dts/overlays/pwm-ir-tx.dtbo                     arch/arm64/boot/dts/overlays/w1-gpio-pullup-pi5.dtbo
arch/arm64/boot/dts/overlays/i2c2-pi5.dtbo                           arch/arm64/boot/dts/overlays/pwm-pio.dtbo                       arch/arm64/boot/dts/overlays/w5500.dtbo
arch/arm64/boot/dts/overlays/i2c3.dtbo                               arch/arm64/boot/dts/overlays/qca7000.dtbo                       arch/arm64/boot/dts/overlays/watterott-display.dtbo
arch/arm64/boot/dts/overlays/i2c3-pi5.dtbo                           arch/arm64/boot/dts/overlays/qca7000-uart0.dtbo                 arch/arm64/boot/dts/overlays/waveshare-can-fd-hat-mode-a.dtbo
arch/arm64/boot/dts/overlays/i2c4.dtbo                               arch/arm64/boot/dts/overlays/ramoops.dtbo                       arch/arm64/boot/dts/overlays/waveshare-can-fd-hat-mode-b.dtbo
arch/arm64/boot/dts/overlays/i2c5.dtbo                               arch/arm64/boot/dts/overlays/ramoops-pi4.dtbo                   arch/arm64/boot/dts/overlays/wifimac.dtbo
arch/arm64/boot/dts/overlays/i2c6.dtbo                               arch/arm64/boot/dts/overlays/rootmaster.dtbo                    arch/arm64/boot/dts/overlays/wittypi.dtbo
arch/arm64/boot/dts/overlays/i2c-bcm2708.dtbo                        arch/arm64/boot/dts/overlays/rotary-encoder.dtbo                arch/arm64/boot/dts/overlays/wm8960-soundcard.dtbo
arch/arm64/boot/dts/overlays/i2c-fan.dtbo                            arch/arm64/boot/dts/overlays/rpi-backlight.dtbo                 arch/arm64/boot/dts/overlays/ws2812-pio.dtbo
a0000@mcalab:~/raspberrypi-linux $ 
```
```bash
a0000@mcalab:~/Desktop $ ls -l /boot/firmware/bcm2712-rpi-5-b.dtb
-rwxr-xr-x 1 root root 78572 Aug  4 00:14 /boot/firmware/bcm2712-rpi-5-b.dtb
a0000@mcalab:~/Desktop $ ls -l /boot/firmware/bcm2712-rpi-5-b.dtb.orig
-rwxr-xr-x 1 root root 78435 Aug  4 00:14 /boot/firmware/bcm2712-rpi-5-b.dtb.orig
a0000@mcalab:~/Desktop $ dmesg | grep -i -E "fdt|device tree"
a0000@mcalab:~/Desktop $ dmesg | grep -i -e "of:" -e "dtb" -e "Raspberry Pi"
[    0.000000] Machine model: Raspberry Pi 5 Model B Rev 1.0
[    0.000000] OF: reserved mem: initialized node linux,cma, compatible id shared-dma-pool
[    0.000000] OF: reserved mem: 0x000000003a000000..0x000000003dffffff (65536 KiB) map reusable linux,cma
[    0.000000] OF: reserved mem: 0x0000000000000000..0x000000000007ffff (512 KiB) nomap non-reusable atf@0
[    0.000000] OF: reserved mem: 0x000000003fd165e0..0x000000003fd16617 (0 KiB) nomap non-reusable nvram@0
[    4.590155] Bluetooth: hci0: BCM43455 37.4MHz Raspberry Pi 3+-0190
a0000@mcalab:~/Desktop $ 
```


---
DTS（Device Tree Source）是一種用於描述嵌入式系統中**硬體資源配置**的文字格式語言，它在 Linux kernel（特別是 ARM 架構）中廣泛應用，用來描述 CPU、記憶體、I/O、匯流排、外設等資訊。

---

## 🔧 基本概念

* `.dts`：Device Tree Source（主檔案）
* `.dtsi`：Device Tree Source Include（可重複使用的共用部分）
* `.dtb`：Device Tree Blob（二進位形式，供 kernel 使用，通常由 `dtc` 編譯而來）

---

## 📜 DTS 語法結構總覽

DTS 類似 C 語言結構，具有層級與節點。每個\*\*節點（node）**代表一個硬體元件，每個**屬性（property）\*\*則定義該元件的設定。

### 📁 Node 範例語法：

```dts
節點名稱@地址 {
    屬性名稱 = 值;
    子節點 {
        子屬性名稱 = 值;
    };
};
```

---

## 📌 常見語法元素

### 1. **節點（Node）**

* 格式：`label: node_name@address { ... }`
* `label`：可選，用於引用（phandle）
* `node_name`：節點名稱
* `@address`：此裝置的地址（通常是記憶體或 I/O 映射地址）

```dts
uart0: serial@4000C000 {
    compatible = "ns16550a";
    reg = <0x4000C000 0x1000>;
    clock-frequency = <24000000>;
    status = "okay";
};
```

---

### 2. **屬性（Property）**

* 格式：`key = value;`
* 文字需加雙引號 `"..."`，數值為 `<...>`（代表 u32），也可用 `[xx xx]` 表示 bytes。

```dts
    compatible = "vendor,device";
    reg = <0x1000 0x100>;         // 起始地址與長度
    interrupts = <5 0>;           // IRQ 資訊
```

---

### 3. **include 其他檔案（`.dtsi`）**

```dts
#include "common.dtsi"
```

---

### 4. **Reference (phandle 與 label)**

* 使用 `&label` 引用其他節點

```dts
&i2c1 {
    status = "okay";
    at24@50 {
        compatible = "atmel,24c256";
        reg = <0x50>;
    };
};
```

---

### 5. **多層次節點**

```dts
soc {
    i2c@40005400 {
        status = "okay";
        eeprom@50 {
            compatible = "atmel,24c256";
            reg = <0x50>;
        };
    };
};
```

---

## 🧠 常見屬性解釋

| 屬性名               | 功能說明                         |
| ----------------- | ---------------------------- |
| `compatible`      | 指定驅動可辨識的字串                   |
| `reg`             | 資源區段（地址與長度）                  |
| `interrupts`      | 中斷號設定                        |
| `status`          | `"okay"`、`"disabled"` 表示啟用與否 |
| `clock-frequency` | 指定時脈頻率                       |
| `phandle`         | 用於參照另一節點                     |

---

## ✅ 範例

```dts
/ {
    model = "MyBoard v1";
    compatible = "myvendor,myboard";

    memory@80000000 {
        device_type = "memory";
        reg = <0x80000000 0x4000000>;  // 64MB RAM
    };

    uart0: serial@4000C000 {
        compatible = "ns16550a";
        reg = <0x4000C000 0x1000>;
        clock-frequency = <24000000>;
        status = "okay";
    };
};
```

---

## 🛠️ 編譯與轉換

使用 `dtc` 編譯 `.dts` 為 `.dtb`：

```bash
dtc -I dts -O dtb -o output.dtb input.dts
```

---

在 Device Tree（DTS）語法中，`soc` 是一個很常見的節點名稱，它代表 **System on Chip（片上系統）** 的意思。

---

## 🔍 `soc` 是什麼？

`soc`（System on Chip）是一個虛擬節點，用來表示整顆晶片上集成的所有內部元件，例如：

* CPU
* I2C/SPI/UART 控制器
* GPIO 控制器
* Timer
* Interrupt Controller
* DMA 等等

---

## 📦 在 Device Tree 中的作用

`soc` 節點並不是硬體本身的裝置，而是一個「容器節點（container node）」，用來組織和分類與 SoC 有關的硬體模組，讓整體結構更有層次、更易於理解與維護。

---

## 📁 範例結構說明

```dts
/ {
    compatible = "raspberrypi,4-model-b";
    model = "Raspberry Pi 4 Model B";

    soc {
        compatible = "simple-bus";
        #address-cells = <1>;
        #size-cells = <1>;
        ranges;

        uart@7e201000 {
            compatible = "brcm,bcm2835-uart";
            reg = <0x7e201000 0x100>;
            interrupts = <2>;
            status = "okay";
        };

        i2c@7e804000 {
            compatible = "brcm,bcm2835-i2c";
            reg = <0x7e804000 0x1000>;
            interrupts = <3>;
            status = "okay";
        };
    };
};
```

---

## 🧠 關鍵知識補充

| 屬性                              | 說明                                            |
| ------------------------------- | --------------------------------------------- |
| `compatible = "simple-bus"`     | 告訴 kernel：這是個容器節點（bus），其內部裝置可以分別初始化           |
| `ranges`                        | 表示地址對應的轉換關係（一般必要）                             |
| `#address-cells`, `#size-cells` | 定義子節點的 `reg` 屬性格式用幾個 cell 表示地址與長度（通常都是 `<1>`） |

---

## ✅ 總結

* `soc` 在 DTS 中是一個邏輯分類節點，代表 SoC 裡的所有控制器與周邊元件。
* 它通常是個 **bus-type node**（匯流排型節點），使用 `simple-bus`。
* 子節點才是真正的硬體裝置，如 UART、I2C、SPI、GPIO 等。

---

下面針對你貼出的那段 DTS 片段，整理出它所用到的**語法結構**與**格式規則**，讓你快速對照：

---

## 1. 節點（node）宣告

```dts
<label>: <node-name>@<unit-address> {
    /* 內部屬性與子節點 */
};
```

* **label**：可選，用來在同檔或其他檔透過 `&label` 參考它（phandle）。
* **node-name**：節點名稱，對應硬體元件類型（像 `i2c`、`uart`）。
* **@unit-address**：節點的「硬體地址」，通常對應 MMIO base address。

每個 node 有一對 `{}`，內部是一系列 **property**（屬性）與可選的**子節點**。

---

## 2. 屬性（property）語法

```dts
<key> = <value1> <value2> … ;
<key> = "<string1>", "<string2>" ;
<key>;
```

* 每一行結尾**必須**有分號 `;`。
* 屬性名稱後面加等號 `=`，再列出一個或多個**值**，最後 `;`。
* **值的型別**：

  * **整數 List**：用尖括號 `<0x021a0000 0x4000>` 表示（每個元素都是 32-bit cell）。
  * **字串 List**：用雙引號 `"..."`，多字串以逗號 `,` 分隔。
  * **reference**：用 `&alias` 從別的節點取 phandle（例如 `&clks`）。
  * **boolean flag**（無值屬性）：直接寫 `status;`（少見，多用 `status = "okay";`）。

---

## 3. 常見特殊屬性

| 屬性                        | 用途                                          |
| ------------------------- | ------------------------------------------- |
| `compatible = "xx","yy";` | 驅動匹配字串                                      |
| `reg = <addr size>;`      | 記憶體映射 address + 長度                          |
| `interrupts = <…>;`       | IRQ 設定（使用宏或數字 cell）                         |
| `clocks = <&alias ID>;`   | 時鐘控制 phandle + clock-id                     |
| `#address-cells = <n>;`   | 定義這個 bus 下子節點 `reg` 用幾個 cell 表示 address     |
| `#size-cells = <n>;`      | 定義子節點 `reg` 用幾個 cell 表示 size                |
| `status = "disabled";`    | 啟用/停用 標記（"okay"/"disabled" 或不見此屬性即預設"okay"） |

---

## 4. phandle 與引用（\&alias）

1. 在某節點最外層加上：

   ```dts
   clks: clock-controller@020c4000 { … }; 
   ```

   這裡 `clks:` 就是定義 alias。

2. 其他節點引用：

   ```dts
   clocks = <&clks IMX6UL_CLK_I2C1>;
   ```

   意思是拿 `clks` 節點的 `IMX6UL_CLK_I2C1` 第 N 號時鐘。

---

## 5. 編譯與 include

* **Include** 共用片段（通常放在最上面）：

  ```dts
  #include "imx6ul-common.dtsi"
  ```
* **編譯**：

  ```bash
  dtc -I dts -O dtb -o imx6ul.dtb imx6ul.dts
  ```

---

### 範例重點對照

```dts
i2c1: i2c@021a0000 {           /* 1. label + node-name@addr */
    #address-cells = <1>;      /* 2. bus 格式設定 */
    #size-cells    = <0>;
    compatible     =           /* 3. 驅動匹配 */
        "fsl,imx6ul-i2c",
        "fsl,imx21-i2c";
    reg            = <         /* 4. MMIO base/size */
        0x021a0000 0x4000>;
    interrupts     = <         /* 5. 中斷設定 */
        GIC_SPI 36 IRQ_TYPE_LEVEL_HIGH>;
    clocks         = <         /* 6. 時鐘引用 */
        &clks IMX6UL_CLK_I2C1>;
    status         = "disabled"; /* 7. 啟用狀態 */
};
```

---

只要抓住這幾個語法格式：

1. **節點宣告**：`label: name@addr { … };`
2. **屬性格式**：`key = <...>;`、`key = "..." ;`
3. **phandle 引用**：`&alias`
4. **特殊屬性**：`#address-cells` / `compatible` / `reg` / `interrupts` / `clocks` / `status`

---
這段是在 **你的板級 DTS 或 Overlay** 裡，用來 **打開** 先前定義的 `i2c1` bus，並 **在其底下** 加入兩顆 I²C 裝置（mag3110、fxls8471）。細節如下：

```dts
&i2c1 {                          /* ① phandle 引用：取用先前 `i2c1:` 那個節點 */
    clock-frequency = <100000>;  /* ② 設定 I²C bus 時脈：100 kHz */
    pinctrl-names = "default";  /* ③ 指定 pinctrl group 名稱（對應 pinctrl-0） */
    pinctrl-0 = <&pinctrl_i2c1>;/* ④ 引用 pinctrl 節點，配置 SCL/SDA 腳位 */
    status = "okay";            /* ⑤ 啟用這條 I²C bus（預設可能是 disabled） */

    /* ---------------------------------------------------------------- */
    mag3110@0e {                 /* ⑥ 子節點：mag3110 感測器，I²C 位址 0x0E */
        compatible = "fsl,mag3110";
        reg        = <0x0e>;     /* I²C address */
        position   = <2>;        /* 自訂屬性（例如放哪個軸的感測）*/
    };

    fxls8471@1e {                /* ⑦ 子節點：fxls8471 感測器，I²C 位址 0x1E */
        compatible = "fsl,fxls8471";
        reg        = <0x1e>;
    };
};
```

### 重點語法

1. **`&i2c1 { … };`**
   用 `&label` 引用已存在的 node（就是你在 Board `.dtsi` 看到的 `i2c1: i2c@021a0000 { … };`），在這裡加屬性或子節點。

2. **`<key> = <value>;`**

   * `<100000>`：整數 cell，用來傳 32-bit 值（這裡是時脈頻率）。
   * `<&pinctrl_i2c1>`：phandle reference，指向另一個 node。

3. **子節點命名**

   * 形式為 `<device-name>@<hex-address>`，表示在該 bus 上、I²C 位址為 `0x<address>`。
   * 裡面一定要有 `compatible`（匹配 driver）和 `reg`（address cell）。

4. **`status = "okay"` vs. `"disabled"`**
   `"okay"`：kernel 啟動時會註冊這條 bus；
   `"disabled"`：跳過不載入。

---

有了這些，你就能：

* 啟用 I²C1、設定速率與 pinmux，
* 並在同一段 DTS 裡直接把感測器（mag3110、fxls8471）「綁上」去，讓 Linux 啟動後自動探測並載入對應 driver。

---

嵌入式 SoC（例如 i.MX6UL）／Linux 系統的開機流程：

---

## 1. 硬體上電 & Boot ROM

1. **Power-On Reset (POR)**
   SoC 上的 POR 電路將各模組拉到復位狀態，然後釋放。
2. **內建 Boot ROM**
   SoC 內建一段不可更改的程式碼，它會：

   * 讀取引腳設定（fuses/strappings），決定從哪種儲存媒介啟動（eMMC、SD 卡、SPI-NOR、UART…）。
   * 將第一階段 Bootloader（SPL）從該媒介的固定偏移載入 On-chip SRAM，並跳轉執行。

---

## 2. 第一期 Bootloader（S-PL／SPL）

* **硬體初步初始化**

  * 配置 Pin-mux（腳位多工）、時鐘（Clock tree）
  * 初始化外部 DRAM 控制器
* **載入第二階段 Bootloader**

  * 將 U-Boot Proper 載入到 DRAM
  * 跳轉執行 U-Boot

---

## 3. 第二期 Bootloader（U-Boot Proper）

1. **環境設定**

   * 解析 environment variables（如 `bootcmd`、`bootargs`）
   * 初始化序列埠（UART）作為 Console，並可供使用者互動
2. **載入 Linux Kernel 與 Device Tree**

   * 從 Flash/SD/eMMC 讀取 `zImage`（或 `uImage`）、`*.dtb`、以及選用的 `initramfs` 到記憶體
3. **執行 Kernel**

   ```bash
   bootz ${kernel_addr} - ${fdt_addr}
   ```

   或使用 `bootm`/`booti`，依映像格式而定，然後將控制權交給 Linux Kernel。

---

## 4. Linux Kernel 啟動

1. **解壓縮**
   Kernel 映像自我解壓到高階記憶體。
2. **Early-init**

   * 切換到 C 環境
   * 初始化中斷控制器（GIC）
   * 建立記憶體管理與核心線程子系統
3. **解析 Device Tree**

   * 讀取 DTB
   * 根據節點（如 `&i2c1`、`reg`、`interrupts` 等），建立 platform device
4. **驅動載入 & 根檔系統掛載**

   * 若有 initramfs，先掛載它作為暫時根檔系統
   * 否則根據 `root=` 參數，從指定分區掛載實際根檔系統

---

## 5. 使用者空間 Init

1. **啟動 `/sbin/init`**（或 `systemd`、`busybox-init` 等）
2. **執行啟動腳本／服務單元**
3. **設定網路、啟動背景服務、顯示登入提示符**

至此，系統即完成從上電到呈現使用者空間介面的全部流程。

---

### 整體流程圖

```
硬體上電
   ↓
Boot ROM → 載入 SPL (Stage1)
   ↓
SPL → 初始化 DRAM & 時鐘 → 載入 U-Boot
   ↓
U-Boot → 初始化外設 → 載入 Kernel+DTB+initrd → bootz/bootm
   ↓
Linux Kernel → 解壓 → 解析 DT → 驅動初始化 → 掛載根 FS
   ↓
Init → 啟動服務 → 使用者空間
```

# **Device Tree** 的結構「畫出來」，重點是想表達下面幾層的關係：

```
(ALPHA)       ←─── 你的 SoC（System on Chip）節點，用一個 label 叫做 ALPHA
  ├─ cpu          ←─── CPU 節點（哪顆核心、頻率等等）
  ├─ clocks       ←─── clock-controller 節點（時鐘源設定）
  └─ bus-tree     ←─── 定義所有匯流排／總線的 container
       ├─ i2c1    ←─── 第一組 I²C 控制器
       ├─ i2c2    ←─── 第二組 I²C 控制器
       └─ …       ←─── 其他 bus (SPI、UART、…)
```

然後圖右邊那個大括號包住的 `mag3110`，箭頭指向 `i2c1`，就是在示意：

> **「這顆 MAG3110（三軸電子羅盤）感測器是接在 i2c1 這條匯流排底下的子節點」**

換句話說，把我們剛剛在 DTS 裡

```dts
&i2c1 {
    …                /* i2c1 bus 的設定 */
    mag3110@0e {     /* i2c1 bus 底下的子節點：address = 0x0e */
        compatible = "fsl,mag3110";
        reg        = <0x0e>;
    };
};
```

用一張樹狀結構圖畫出來而已。

---

### 重點回顧

1. **最頂層**（ALPHA）代表你的 SoC 或 root node。
2. **cpu / clocks** 屬於 SoC 內建硬體資源。
3. **bus-tree**（simple-bus）是個「容器節點」，把所有匯流排都放在一起。
4. **i2c1 / i2c2 …** 都是 bus-tree 底下的節點，各自對應一組 I²C 控制器。
5. **mag3110\@0e** 則是掛在 i2c1 底下的子節點，代表這顆感測器的 I²C 位址是 0x0E。

這樣的圖就是把 DTS 的文字結構，轉成一目了然的層級示意圖，幫助理解整個硬體拓樸（topology）與 device tree 的對應關係。


