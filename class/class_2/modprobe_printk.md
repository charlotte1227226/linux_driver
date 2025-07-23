# modprobe完整流程，直接在任意目錄下執行即可：

1. **編譯你的模組**

   ```bash
   make
   ```

   → 產生 `chrdevbase.ko`

2. **複製到系統模組目錄**

   ```bash
   sudo cp chrdevbase.ko /lib/modules/$(uname -r)/kernel/drivers/misc/
   ```

3. **更新模組依賴索引**

   ```bash
   sudo depmod -a
   ```

4. **載入模組（使用 modprobe）**

   ```bash
   sudo modprobe chrdevbase
   ```

5. **確認模組已載入**

   ```bash
   lsmod | grep chrdevbase
   ```

6. **查看 printk 日誌輸出**

   ```bash
   dmesg | tail -n 10
   ```

7. **卸載模組**

   ```bash
   sudo modprobe -r chrdevbase
   ```

   或

   ```bash
   sudo rmmod chrdevbase
   ```

   → 再用 `dmesg | tail -n 5` 確認 exit 訊息

---

> **小提示**
>
> * 放在 `drivers/misc/` 只是慣例，確保路徑在 `/lib/modules/$(uname -r)/kernel/...` 下即可。
> * 之後要實作字元設備，才需要在 `init`/`exit` 中註冊 `alloc_chrdev_region`、`cdev_add`、`device_create` 等。

---

這條指令其實是把 **Kernel 訊息緩衝區** 的最新內容，擷取出 **最後 10 行** 來看：

1. `dmesg`

   * 讀取並顯示 Linux 核心的 **ring buffer**（環形緩衝區）裡所有訊息，常見用來看開機時或模組 (`printk`) 輸出的 log。

2. `|`（管線）

   * 將前面 `dmesg` 的輸出「導到」後面這個指令去處理。

3. `tail -n 10`

   * `tail` 預設會顯示輸入的最後 10 行；加上 `-n 10` 明確指定要最後 10 行。

---

### 為什麼常這樣用？

* **檢查模組載入／卸載訊息**：當你 `insmod`、`rmmod` 或 `modprobe` 之後，裡面用 `printk` 印的資訊會存在 dmesg，直接看最後幾行就能立刻知道有沒有成功、或錯在哪。
* **排除雜訊**：完整的 `dmesg` 常常很長，管線到 `tail -n 10` 可以快速抓最新的重點訊息。

---

#### 範例

```bash
sudo insmod mymod.ko
dmesg | tail -n 10
# 立刻看到 mymod init 時 printk 出來的訊息
```

這樣你就不用翻整個 kernel log，只要看最末端的 10 條！

---

# printk

1. **為什麼用 `printk`**

   * `printf` 是使用者空間的函式，內核裡不存在(但是還是可以用)。
   * 內核要輸出訊息到控制台或 log，就必須用 `printk`。

2. **八個訊息等級定義**（都定義在 `include/linux/kern_levels.h`）

   ```c
   #define KERN_SOH     "\001"       // SOH = start of header, 用來標示接下來的數字是等級
   #define KERN_EMERG   KERN_SOH "0" // 緊急事件（kernel panic）
   #define KERN_ALERT   KERN_SOH "1" // 必須立即處理
   #define KERN_CRIT    KERN_SOH "2" // 臨界條件（嚴重 SW 或 HW 錯誤）
   #define KERN_ERR     KERN_SOH "3" // 錯誤狀態（常用於裝置驅動）
   #define KERN_WARNING KERN_SOH "4" // 警告，不會造成嚴重影響
   #define KERN_NOTICE  KERN_SOH "5" // 一般提示性訊息
   #define KERN_INFO    KERN_SOH "6" // 資訊性訊息
   #define KERN_DEBUG   KERN_SOH "7" // 偵錯用訊息
   ```

   * 等級由 0（最高優先，最嚴重）到 7（最低優先，純 debug）共 8 級。

3. **使用範例**

   ```c
   printk(KERN_INFO "driver initialized\n");
   ```

   這行就會以「資訊等級」印出 `"driver initialized"`。

---

**重點**：在編寫核心模組時，要透過 `printk` 加上適當的 `KERN_<LEVEL>` 前綴，才能正確地分類與顯示日誌訊息。


---

```bash
a0000@mcalab:~/Desktop/linux_driver/class/class_2 $ uname -r
6.12.34+rpt-rpi-2712
a0000@mcalab:~/Desktop/linux_driver/class/class_2 $ sudo cp chrdevbase.ko /lib/modules/$(uname -r)/kernel/drivers/misc/
a0000@mcalab:~/Desktop/linux_driver/class/class_2 $ sudo depmod -a
a0000@mcalab:~/Desktop/linux_driver/class/class_2 $ sudo modprobe chrdevbase
a0000@mcalab:~/Desktop/linux_driver/class/class_2 $ lsmod
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
hci_uart               65536  0
btbcm                  49152  1 hci_uart
brcmfmac              376832  1 brcmfmac_wcc
bluetooth             638976  31 hci_uart,btbcm,bnep,rfcomm
brcmutil               49152  1 brcmfmac
rpi_hevc_dec           65536  0
cfg80211             1032192  1 brcmfmac
hid_multitouch         49152  0
aes_ce_blk             49152  4
ecdh_generic           49152  2 bluetooth
aes_ce_cipher          49152  1 aes_ce_blk
v4l2_mem2mem           65536  1 rpi_hevc_dec
ghash_ce               49152  0
pisp_be                49152  0
gf128mul               49152  1 ghash_ce
sha2_ce                49152  0
ecc                    65536  1 ecdh_generic
sha256_arm64           49152  1 sha2_ce
sha1_ce                49152  0
videobuf2_dma_contig    49152  2 pisp_be,rpi_hevc_dec
videobuf2_memops       49152  1 videobuf2_dma_contig
videobuf2_v4l2         49152  3 pisp_be,rpi_hevc_dec,v4l2_mem2mem
libaes                 49152  4 aes_ce_cipher,bluetooth,ghash_ce,aes_ce_blk
sha1_generic           49152  1 sha1_ce
rfkill                 49152  6 bluetooth,cfg80211
videodev              344064  4 pisp_be,rpi_hevc_dec,videobuf2_v4l2,v4l2_mem2mem
raspberrypi_hwmon      49152  0
videobuf2_common       98304  6 pisp_be,rpi_hevc_dec,videobuf2_dma_contig,videobuf2_v4l2,v4l2_mem2mem,videobuf2_memops
mc                     81920  6 videodev,pisp_be,rpi_hevc_dec,videobuf2_v4l2,videobuf2_common,v4l2_mem2mem
rp1_pio                65536  0
i2c_brcmstb            49152  0
gpio_keys              49152  0
pwm_fan                49152  0
spi_bcm2835            49152  0
i2c_designware_platform    49152  0
i2c_designware_core    49152  1 i2c_designware_platform
rp1_mailbox            49152  1
rp1_fw                 49152  1 rp1_pio
spi_dw_mmio            49152  0
rp1_adc                49152  0
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
vc4                   425984  18
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
drm_shmem_helper       49152  1 v3d
cec                    65536  1 vc4
drm_kms_helper        245760  3 drm_dma_helper,vc4,drm_shmem_helper
gpu_sched              98304  1 v3d
drm                   688128  20 gpu_sched,drm_kms_helper,drm_dma_helper,v3d,vc4,drm_shmem_helper,drm_display_helper
drm_panel_orientation_quirks    65536  1 drm
backlight              49152  2 drm_kms_helper,drm
uio_pdrv_genirq        49152  0
uio                    49152  1 uio_pdrv_genirq
a0000@mcalab:~/Desktop/linux_driver/class/class_2 $ dmesg | tail -n 10
[    7.480248] macb 1f00100000.ethernet: gem-ptp-timer ptp clock registered.
[    7.490367] brcmfmac: brcmf_cfg80211_set_power_mgmt: power save enabled
[    9.645613] Bluetooth: RFCOMM TTY layer initialized
[    9.645624] Bluetooth: RFCOMM socket layer initialized
[    9.645631] Bluetooth: RFCOMM ver 1.11
[ 1366.752242] chrdevbase: loading out-of-tree module taints kernel.
[ 1366.752680] chrdevbase_init
[ 1515.288965] chrdevbese_exit
[ 1580.852252] logitech-hidpp-device 0003:046D:4091.0005: HID++ 4.5 device connected.
[ 1948.753456] chrdevbase_init
a0000@mcalab:~/Desktop/linux_driver/class/class_2 $ sudo modprobe -r chrdevbase
a0000@mcalab:~/Desktop/linux_driver/class/class_2 $ dmesg | tail -n 5
[ 1366.752680] chrdevbase_init
[ 1515.288965] chrdevbese_exit
[ 1580.852252] logitech-hidpp-device 0003:046D:4091.0005: HID++ 4.5 device connected.
[ 1948.753456] chrdevbase_init
[ 2080.817236] chrdevbese_exit
a0000@mcalab:~/Desktop/linux_driver/class/class_2 $ 
```