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

# 裝置節點
在 Linux 裡，「裝置節點（device node，或叫做 device file）」其實就是一種特殊的檔案，它位於 `/dev` 目錄底下，用來作為使用者空間跟核心空間中驅動程式溝通的「橋樑」。當你執行：

```bash
sudo mknod /dev/chrdevbase c 200 0
```

— 這條指令就是在 `/dev` 下建立一個名為 `chrdevbase` 的字元（character）裝置節點，並且指定：

* **類型 `c`**：告訴系統這是一個字元裝置（一次讀／寫一個 byte）
* **major number = 200**：內核用這個編號去找到對應註冊在核心裡的驅動程式
* **minor number = 0**：讓同一個驅動程式底下可以區分多個「子裝置」或不同功能

---

### 為什麼需要裝置節點？

1. **使用者空間要開啟、讀寫驅動**
   程式在使用者空間呼叫 `open("/dev/chrdevbase", O_RDWR)` 時，內核會先從檔案系統確認這是一個字元裝置，接著讀出它的 major/minor，
   2\. 再把這個 open 呼叫「路由」到對應 major=200 的驅動程式裡（也就是你 `register_chrdev(200, ...)` 時註冊的那隻）。
2. **read/write/ioctl 都經這個節點**
   之後對這個檔案執行 `read()`、`write()` 或 `ioctl()`，核心就會呼叫你驅動裡對應的 `read`、`write`、`unlocked_ioctl` 等 callback，
   3\. 讓使用者「像操作一般檔案一樣」，去跟硬體或模擬硬體的驅動程式互動。

---

### 裝置節點 vs. 真正的檔案

* **普通檔案**（regular file）裡有資料內容存在磁碟上；
* **裝置節點**本身不存資料，它只是一個「入口」。當你存取它，底層驅動才會做實際動作（讀感測器，或者寫到硬體暫存器、回傳模擬資料）。

---

### 小結

* **建立裝置節點** = 在 `/dev` 下放一個特殊檔案，告訴系統：

  * 這是字元（`c`）還是區塊（`b`）裝置
  * 它的 major/minor 號是多少
* **作用** = 讓使用者空間的程式能透過 `open`/`read`/`write` 去操作你在核心裡寫的驅動程式

因此，當你每次開機後 `/dev/chrdevbase` 消失，就代表那個「橋樑」沒了，程式就無法透過檔案介面跟你的驅動溝通，必須重新 `mknod` 或讓 udev／驅動自動重建。

---
```
┌─────────────────────────────────────────────────────────┐
│                     使用者空間 (User Space)              │
│                                                         │
│  +--------------------+                                 │
│  |   chrdevbaseAPP    |───open("/dev/chrdevbase")─────┐ │
│  |  (read/write)      |                                 │
│  +--------------------+                                 │
└─────────────────────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│                   虛擬檔案系統 (VFS)                     │
│                                                         │
│  ┌───────────────────────────────────────────────────┐  │
│  │ /dev/chrdevbase（字元裝置節點）                     │   │
│  │   • 類型：c                                      │ │
│  │   • major = 200                                  │ │
│  │   • minor = 0                                    │ │
│  └───────────────────────────────────────────────────┘ │
│                   │                                  │
│                   ▼                                  │
│  VFS 根據 major/minor 調度到對應驅動 ——> file_operations │
└─────────────────────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│                   核心空間 (Kernel Space)              │
│                                                     │
│  +-------------------------------------------------+ │
│  | chrdevbase_init()                               | │
│  |  register_chrdev(200, "chrdevbase", &chrdevbase_fops) | │
│  +-------------------------------------------------+ │
│              ▲      ▲        ▲        ▲             │
│              │      │        │        │             │
│    ┌─────────┘      │        │        └───────────┐ │
│    │                │        │                    │ │
│    ▼                ▼        ▼                    ▼ │
│  chrdevbase_open   chrdevbase_read          chrdevbase_write   chrdevbase_release
│  （open 回調）     （read 回調）            （write 回調）      （release 回調）
│    │                │        │                    │ │
│    │                │        │                    │ │
│    └─────────┐      │        │        ┌───────────┘ │
│              ▼      ▼        ▼        ▼             │
│  驅動內部：copy_to_user / memcpy / copy_from_user     │
└─────────────────────────────────────────────────────────┘
```

---
# 最終結果

```bash
a0000@mcalab:~/Desktop/linux_driver/class/class_5 $ make
make -C /lib/modules/6.12.25+rpt-rpi-2712/build M=/home/a0000/Desktop/linux_driver/class/class_5 modules
make[1]: Entering directory '/home/a0000/kernel-src/linux'
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_5/chrdevbase.o
  MODPOST /home/a0000/Desktop/linux_driver/class/class_5/Module.symvers
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_5/chrdevbase.mod.o
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_5/.module-common.o
  LD [M]  /home/a0000/Desktop/linux_driver/class/class_5/chrdevbase.ko
make[1]: Leaving directory '/home/a0000/kernel-src/linux'
a0000@mcalab:~/Desktop/linux_driver/class/class_5 $ sudo cp chrdevbase.ko /lib/modules/$(uname -r)/kernel/drivers/misc/
a0000@mcalab:~/Desktop/linux_driver/class/class_5 $ gcc ./chrdevbaseAPP.c -o chrdevbaseAPP
a0000@mcalab:~/Desktop/linux_driver/class/class_5 $ ls /lib/modules/$(uname -r)/kernel/drivers/misc/
bcm2835_smi.ko.xz  chrdevbase.ko  eeprom  rp1-pio.ko.xz  ti-st  ws2812-pio-rp1.ko.xz
a0000@mcalab:~/Desktop/linux_driver/class/class_5 $ sudo modprobe chrdevbase
a0000@mcalab:~/Desktop/linux_driver/class/class_5 $ lsmod
Module                  Size  Used by
chrdevbase             49152  0
rfcomm                 81920  4
algif_hash             49152  1
algif_skcipher         49152  1
af_alg                 49152  6 algif_hash,algif_skcipher
bnep                   49152  2
binfmt_misc            49152  1
brcmfmac_wcc           49152  0
spidev                 49152  0
brcmfmac              376832  1 brcmfmac_wcc
hci_uart               65536  0
btbcm                  49152  1 hci_uart
bluetooth             638976  31 hci_uart,btbcm,bnep,rfcomm
brcmutil               49152  1 brcmfmac
aes_ce_blk             49152  4
cfg80211             1032192  1 brcmfmac
aes_ce_cipher          49152  1 aes_ce_blk
ghash_ce               49152  0
pisp_be                49152  0
rpi_hevc_dec           65536  0
gf128mul               49152  1 ghash_ce
ecdh_generic           49152  2 bluetooth
sha2_ce                49152  0
sha256_arm64           49152  1 sha2_ce
ecc                    65536  1 ecdh_generic
sha1_ce                49152  0
libaes                 49152  4 aes_ce_cipher,bluetooth,ghash_ce,aes_ce_blk
hid_multitouch         49152  0
v4l2_mem2mem           65536  1 rpi_hevc_dec
videobuf2_dma_contig    49152  2 pisp_be,rpi_hevc_dec
rfkill                 49152  6 bluetooth,cfg80211
videobuf2_memops       49152  1 videobuf2_dma_contig
sha1_generic           49152  1 sha1_ce
videobuf2_v4l2         49152  3 pisp_be,rpi_hevc_dec,v4l2_mem2mem
raspberrypi_hwmon      49152  0
videodev              344064  4 pisp_be,rpi_hevc_dec,videobuf2_v4l2,v4l2_mem2mem
videobuf2_common       98304  6 pisp_be,rpi_hevc_dec,videobuf2_dma_contig,videobuf2_v4l2,v4l2_mem2mem,videobuf2_memops
mc                     81920  6 videodev,pisp_be,rpi_hevc_dec,videobuf2_v4l2,videobuf2_common,v4l2_mem2mem
gpio_keys              49152  0
i2c_brcmstb            49152  0
spi_bcm2835            49152  0
rp1_pio                65536  0
pwm_fan                49152  0
i2c_designware_platform    49152  0
i2c_designware_core    49152  1 i2c_designware_platform
rp1_mailbox            49152  1
spi_dw_mmio            49152  0
rp1_adc                49152  0
rp1_fw                 49152  1 rp1_pio
raspberrypi_gpiomem    49152  0
spi_dw                 49152  1 spi_dw_mmio
nvmem_rmem             49152  0
i2c_dev                49152  0
ledtrig_pattern        49152  0
fuse                  196608  5
dm_mod                163840  0
ip_tables              65536  0
x_tables               81920  1 ip_tables
ipv6                  606208  52
hid_logitech_hidpp     65536  0
hid_logitech_dj        49152  0
joydev                 49152  0
vc4                   425984  17
snd_soc_hdmi_codec     49152  2
snd_soc_core          311296  2 vc4,snd_soc_hdmi_codec
snd_compress           49152  1 snd_soc_core
snd_pcm_dmaengine      49152  1 snd_soc_core
snd_pcm               147456  4 snd_soc_hdmi_codec,snd_compress,snd_soc_core,snd_pcm_dmaengine
snd_timer              65536  1 snd_pcm
snd                   131072  9 snd_soc_hdmi_codec,snd_timer,snd_compress,snd_soc_core,snd_pcm
v3d                   212992  8
drm_display_helper     49152  1 vc4
drm_dma_helper         49152  2 vc4
cec                    65536  1 vc4
drm_shmem_helper       49152  1 v3d
gpu_sched              98304  1 v3d
drm_kms_helper        245760  3 drm_dma_helper,vc4,drm_shmem_helper
drm                   688128  20 gpu_sched,drm_kms_helper,drm_dma_helper,v3d,vc4,drm_shmem_helper,drm_display_helper
drm_panel_orientation_quirks    65536  1 drm
backlight              49152  2 drm_kms_helper,drm
uio_pdrv_genirq        49152  0
uio                    49152  1 uio_pdrv_genirq
a0000@mcalab:~/Desktop/linux_driver/class/class_5 $ cat /proc/devices
Character devices:
  1 mem
  4 /dev/vc/0
  4 tty
  4 ttyS
  5 /dev/tty
  5 /dev/console
  5 /dev/ptmx
  5 ttyprintk
  7 vcs
 10 misc
 13 input
 14 sound
 29 fb
 81 video4linux
 89 i2c
116 alsa
128 ptm
136 pts
153 spi
180 usb
189 usb_device
200 chrdevbase
204 ttyAMA
216 rfcomm
226 drm
234 gpiomem0
235 gpiomem4
236 gpiomem3
237 gpiomem2
238 gpiomem1
239 cec
240 uio
241 binder
242 hidraw
243 rpmb
244 nvme-generic
245 nvme
246 vc-mem
247 bsg
248 watchdog
249 ptp
250 pps
251 lirc
252 rtc
253 dma_heap
254 gpiochip
510 media
511 rp1-pio

Block devices:
  1 ramdisk
  7 loop
  8 sd
 65 sd
 66 sd
 67 sd
 68 sd
 69 sd
 70 sd
 71 sd
128 sd
129 sd
130 sd
131 sd
132 sd
133 sd
134 sd
135 sd
179 mmc
254 device-mapper
259 blkext
a0000@mcalab:~/Desktop/linux_driver/class/class_5 $ dmesg | tail -n 10
[    5.449716] macb 1f00100000.ethernet eth0: PHY [1f00100000.ethernet-ffffffff:01] driver [Broadcom BCM54213PE] (irq=POLL)
[    5.449725] macb 1f00100000.ethernet eth0: configuring for phy/rgmii-id link mode
[    5.452613] macb 1f00100000.ethernet: gem-ptp-timer ptp clock registered.
[    5.463475] brcmfmac: brcmf_cfg80211_set_power_mgmt: power save enabled
[    7.599674] Bluetooth: RFCOMM TTY layer initialized
[    7.599685] Bluetooth: RFCOMM socket layer initialized
[    7.599692] Bluetooth: RFCOMM ver 1.11
[ 1601.126784] logitech-hidpp-device 0003:046D:4091.0005: HID++ 4.5 device connected.
[ 2837.812768] chrdevbase: loading out-of-tree module taints kernel.
[ 2837.813347] chrdevbase_init
a0000@mcalab:~/Desktop/linux_driver/class/class_5 $ sudo mknod /dev/chrdevbase c 200 0
a0000@mcalab:~/Desktop/linux_driver/class/class_5 $ sudo ./chrdevbaseAPP /dev/chrdevbase 1
Correct usage! argc = 3
Open file /dev/chrdevbase success
file_descriptor = 3 read file /dev/chrdevbase success
ret = 0 
APP read data: kernel data! 
close file /dev/chrdevbase success
ret = 0 
a0000@mcalab:~/Desktop/linux_driver/class/class_5 $ dmesg | tail -n 10
[    5.452613] macb 1f00100000.ethernet: gem-ptp-timer ptp clock registered.
[    5.463475] brcmfmac: brcmf_cfg80211_set_power_mgmt: power save enabled
[    7.599674] Bluetooth: RFCOMM TTY layer initialized
[    7.599685] Bluetooth: RFCOMM socket layer initialized
[    7.599692] Bluetooth: RFCOMM ver 1.11
[ 1601.126784] logitech-hidpp-device 0003:046D:4091.0005: HID++ 4.5 device connected.
[ 2837.812768] chrdevbase: loading out-of-tree module taints kernel.
[ 2837.813347] chrdevbase_init
[ 3079.651159] chrdevbase_open
[ 3079.651206] chrdevbase_release
a0000@mcalab:~/Desktop/linux_driver/class/class_5 $ sudo ./chrdevbaseAPP /dev/chrdevbase 2
Correct usage! argc = 3
Open file /dev/chrdevbase success
file_descriptor = 3 write file /dev/chrdevbase success
ret = 0 
close file /dev/chrdevbase success
ret = 0 
a0000@mcalab:~/Desktop/linux_driver/class/class_5 $ dmesg | tail -n 10
[    7.599685] Bluetooth: RFCOMM socket layer initialized
[    7.599692] Bluetooth: RFCOMM ver 1.11
[ 1601.126784] logitech-hidpp-device 0003:046D:4091.0005: HID++ 4.5 device connected.
[ 2837.812768] chrdevbase: loading out-of-tree module taints kernel.
[ 2837.813347] chrdevbase_init
[ 3079.651159] chrdevbase_open
[ 3079.651206] chrdevbase_release
[ 3452.497654] chrdevbase_open
[ 3452.497666] kernel recevdata:user data!
[ 3452.497672] chrdevbase_release
a0000@mcalab:~/Desktop/linux_driver/class/class_5 $ 
```

---
## 補充說明

`Module.symvers` 是 Linux 核心建置系統（Kbuild）在「modpost」階段所產生的一個符號版本檔，用來管理模組間的符號（symbol）匯出與版本相容性。它的意義和作用可以分成以下幾點：

---

## 1. 內容

* 每一行代表一個「已匯出符號（exported symbol）」
* 格式大致是：

  ```
  <symbol_name>   <CRC32_of_symbol_signature>   <origin_module_or_vmlinux>
  ```

  例如：

  ```
  copy_to_user   1a2b3c4d   vmlinux
  printk         ab12cd34   kernel/printk.o
  ```

---

## 2. 生成時機

1. **編譯核心或原生模組時**

   * 在 `make modules` 或 `make` 的最後階段，Kbuild 會呼叫 `scripts/mod/modpost`
2. **modpost** 會收集：

   * vmlinux（核心本身）的所有 EXPORT\_SYMBOL
   * 以及目前樹狀下所有模組的 EXPORT\_SYMBOL
3. **輸出 `Module.symvers`** 到核心根目錄，以及各子目錄下（若有 sub‑Makefile）

---

## 3. 主要用途

1. **符號版本檢查（Symbol Versioning）**

   * CRC32 可以保證你連結到的符號「原型／API」跟核心版本是一致的
   * 若 external module 中用到的符號 CRC 跟 `Module.symvers` 裡不符，modpost 會警告或錯誤
2. **跨目錄／out‑of‑tree 模組建置**

   * 當你在外部資料夾（不在 kernel tree 裡）編譯自己的模組時，可用：

     ```bash
     make -C /lib/modules/$(uname -r)/build \
         M=$PWD \
         KBUILD_EXTRA_SYMBOLS=/path/to/Module.symvers \
         modules
     ```
   * 這樣外部模組就能「看見」核心或其他模組所匯出的符號與版本資訊
3. **避免未定義符號**

   * 如果沒有 `Module.symvers`，外部模組只會搜尋 vmlinux 本身的 EXPORT\_SYMBOL，找不到就建置失敗
   * 若有但版本不符，也會在連結階段被攔截，早於執行時崩潰

---

## 4. 小結架構圖

```
┌────────────────────────────┐
│  Kernel Tree /build 目錄   │
│                            │
│  ┌───────────┐   ┌───────┐ │
│  │  vmlinux  │   │ modules ├───┐
│  └───────────┘   └───────┘ │   │
│       │             │      │   │
│       ▼             ▼      │   ▼
│   scripts/mod/modpost      │  Module.symvers
│       │                    │   ────┐
│       ▼                    │       │
│   收集 EXPORT_SYMBOL       │       │
│       │                    │       │
│       └───> 輸出 Module.symvers ──┘
└────────────────────────────┘

  ↑  
  │
  │ 用於 out‑of‑tree 模組建置時，透過
  │ KBUILD_EXTRA_SYMBOLS 指定
  │
  ↓

┌───────────────────────────┐
│ External Module Source    │
│  (out‑of‑tree)            │
│  make … KBUILD_EXTRA_SYMBOLS=/…/Module.symvers │
└───────────────────────────┘
```

**總之，`Module.symvers` 的核心意義就是維護「模組符號 + 版本」清單，讓所有模組在編譯連結階段就能正確解析並檢查版本相容性，避免執行時的符號不匹配問題。**

---
# make 跟 cmake的用途和意義和差別是什麼

以下用架構化方式說明 Make 和 CMake 的用途、意義，以及它們之間的差別。

---

## 一、Make

1. **定位**

   * 傳統的「建置工具」（build tool），直接讀取 Makefile 中定義的「規則」（rule）與「相依性」（dependency），執行命令來編譯／連結程式。

2. **主要特色**

   * **命令式**：每個 target 都寫明「怎麼做」
   * **相依性追蹤**：根據檔案時間戳判斷哪些檔案要重建
   * **單一檔案格式**：Makefile
   * **平台依賴**：多半寫死在 Unix-like 環境裡（shell 指令、gcc、ar…）

3. **使用範例**

   ```makefile
   CC = gcc
   CFLAGS = -Wall -g

   all: app

   app: main.o util.o
       $(CC) $(CFLAGS) -o $@ $^

   %.o: %.c
       $(CC) $(CFLAGS) -c $<

   clean:
       rm -f *.o app
   ```

   * 執行 `make` 就會依相依性自動編譯、連結。

---

## 二、CMake

1. **定位**

   * 「跨平台建置系統產生器」（meta build system generator），透過一套高階指令（CMakeLists.txt），自動產生對應平台的 Makefile、Ninja file、或 Visual Studio solution。

2. **主要特色**

   * **宣告式／高階**：用 `add_executable()`、`target_link_libraries()` 等指令描述「要做什麼」，不必管底層怎麼呼叫編譯器。
   * **跨平台**：同一份 CMakeLists.txt 可在 Linux、macOS、Windows、嵌入式交叉編譯環境… 輕鬆重用。
   * **支援 out‑of‑source build**：可把建置檔案都放在專案外的建置目錄，提高原始碼目錄整潔度。
   * **豐富功能**：自動尋找第三方函式庫（FindPackage）、產生安裝規則（install）、產生測試規則（CTest）等。

3. **使用範例**

   ```cmake
   cmake_minimum_required(VERSION 3.10)
   project(MyApp C)

   set(CMAKE_C_STANDARD 11)
   set(CMAKE_C_FLAGS_DEBUG "-g -Wall")

   add_executable(app main.c util.c)
   target_include_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/include)
   ```

   * 在專案根目錄執行：

     ```bash
     mkdir build && cd build
     cmake ..        # 生成 Makefile 或 Ninja 檔
     make            # 或 ninja，進行實際建置
     ```

---

## 三、主要差別

| 特性        | Make                        | CMake                         |
| --------- | --------------------------- | ----------------------------- |
| **角色**    | 直接執行建置命令的工具                 | 產生平台原生建置腳本（Makefile/Ninja/VS） |
| **檔案格式**  | Makefile                    | CMakeLists.txt                |
| **指令風格**  | 命令式（shell script）           | 宣告式（高階指令）                     |
| **跨平台能力** | 需手動改寫不同 OS/編譯器參數            | 同一份 CMakeLists.txt 支援多平台      |
| **相依性管理** | 透過檔案時間戳與顯式規則                | 先產生再由底層建置工具（Make/Ninja）處理     |
| **建置目錄**  | 通常 in‑source（建置檔與原始碼混放）     | 支援 out‑of‑source（建置檔與原始碼分離）   |
| **擴充生態**  | 限於 shell / 自定義 Makefile 函式庫 | 丰富模組：FindXYZ、CTest、CPack 安裝套件 |

---

### 四、何時選用？

* **小型／單平台專案**：

  * 只有幾個源檔、目標環境固定，用 Make 直接、簡單即可。
* **大型／多平台專案**：

  * 涉及多平台／多編譯器、第三方函式庫、測試與安裝，CMake 能極大減少跨平台維護成本。

---

**總結**：

* **Make** 是「舊牌但靈活」的直接建置工具，適合簡潔、精細控制的場景；
* **CMake** 是「跨平台的產生器」，用更高階的宣告式語法描述建置流程，能自動化產生符合不同環境需求的原生建置腳本。

---
## `cmake ..` 意思
「..」在 Unix/Linux 的檔案路徑中就是「父目錄」（parent directory）的縮寫。

假設你的專案目錄結構如下：

```
my_project/
├── CMakeLists.txt      ← 這裡放的是你的 CMake 腳本
├── src/
└── build/              ← 建置輸出目錄（還沒生成任何檔案）
```

當你在終端機依序執行：

```bash
mkdir build
cd build
cmake ..
```

1. **`mkdir build`**
   在專案根目錄下建立一個叫 `build` 的子目錄。
2. **`cd build`**
   切換到這個新建立的 `build` 目錄。
3. **`cmake ..`**

   * `..` 就是告訴 CMake：「我的 CMakeLists.txt 和原始程式碼都在當前目錄（`build`）的上層，也就是 `build` 這個資料夾的外面。」
   * CMake 會到父目錄（`my_project/`）去讀取 `CMakeLists.txt`，再在你現在的 `build/` 裡面寫入生成的 Makefile（或 Ninja 設定檔）、編譯中間檔、設定檔等等。
   * 這種「**out‑of‑source build**」（原始碼與建置檔分離）的方式，有助於讓你的原始碼資料夾保持乾淨，並且可以同時為同一份原始碼／CMakeLists.txt 做多組不同的建置（例如 debug／release）。

換句話說，**`cmake ..`** = “在這個目錄（`build/`）裡請 CMake 根據 `build` 的上層目錄（`..`）的 CMakeLists.txt 幫我產生原生的建置系統檔案”。

你也可以把 `..` 換成任何含有 CMakeLists.txt 的路徑，例如：

```bash
cmake /home/a0000/Desktop/my_project
```

但習慣上用相對路徑 `..` 最簡潔。
