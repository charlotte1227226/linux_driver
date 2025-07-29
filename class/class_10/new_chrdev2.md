# 新字符設備驅動框架

1. 字符設備註冊
    - `cdev`表示字符設備
    - 使用`cdev_init`函數來初始化
    ```c
    void cdev_init(struct cdev *cdev, const struct file_operations *fops);
    ```
`cdev_init`初始化完成`cdev`以後，使用`cdev_add`添加到linux 內核。

---
# 終端機結果
```bash
a0000@mcalab:~/Desktop/linux_driver/class/class_10 $ sudo cp newchrled.ko /lib/modules/$(uname -r)/kernel/drivers/misc/
a0000@mcalab:~/Desktop/linux_driver/class/class_10 $ ls /lib/modules/$(uname -r)/kernel/drivers/misc/
bcm2835_smi.ko.xz  chrdevbase.ko  eeprom  led.ko  newchrled.ko  rp1-pio.ko.xz  rpi_led.ko  ti-st  ws2812-pio-rp1.ko.xz
a0000@mcalab:~/Desktop/linux_driver/class/class_10 $ sudo depmod -a
a0000@mcalab:~/Desktop/linux_driver/class/class_10 $ sudo modprobe newchrled
a0000@mcalab:~/Desktop/linux_driver/class/class_10 $ dmesg | tail -n 10
[    4.328660] NET: Registered PF_ALG protocol family
[    5.061577] macb 1f00100000.ethernet eth0: PHY [1f00100000.ethernet-ffffffff:01] driver [Broadcom BCM54213PE] (irq=POLL)
[    5.061586] macb 1f00100000.ethernet eth0: configuring for phy/rgmii-id link mode
[    5.064753] macb 1f00100000.ethernet: gem-ptp-timer ptp clock registered.
[    5.074408] brcmfmac: brcmf_cfg80211_set_power_mgmt: power save enabled
[    7.192671] Bluetooth: RFCOMM TTY layer initialized
[    7.192682] Bluetooth: RFCOMM socket layer initialized
[    7.192689] Bluetooth: RFCOMM ver 1.11
[ 3492.633679] newchrled: loading out-of-tree module taints kernel.
[ 3492.634314] newchrled_init
a0000@mcalab:~/Desktop/linux_driver/class/class_10 $ lsmod
Module                  Size  Used by
newchrled              49152  0
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
bluetooth             638976  31 hci_uart,btbcm,bnep,rfcomm
brcmfmac              376832  1 brcmfmac_wcc
brcmutil               49152  1 brcmfmac
cfg80211             1032192  1 brcmfmac
hid_multitouch         49152  0
aes_ce_blk             49152  4
rpi_hevc_dec           65536  0
pisp_be                49152  0
aes_ce_cipher          49152  1 aes_ce_blk
ghash_ce               49152  0
gf128mul               49152  1 ghash_ce
sha2_ce                49152  0
v4l2_mem2mem           65536  1 rpi_hevc_dec
ecdh_generic           49152  2 bluetooth
sha256_arm64           49152  1 sha2_ce
videobuf2_dma_contig    49152  2 pisp_be,rpi_hevc_dec
sha1_ce                49152  0
videobuf2_memops       49152  1 videobuf2_dma_contig
videobuf2_v4l2         49152  3 pisp_be,rpi_hevc_dec,v4l2_mem2mem
ecc                    65536  1 ecdh_generic
videodev              344064  4 pisp_be,rpi_hevc_dec,videobuf2_v4l2,v4l2_mem2mem
rfkill                 49152  6 bluetooth,cfg80211
sha1_generic           49152  1 sha1_ce
libaes                 49152  4 aes_ce_cipher,bluetooth,ghash_ce,aes_ce_blk
raspberrypi_hwmon      49152  0
videobuf2_common       98304  6 pisp_be,rpi_hevc_dec,videobuf2_dma_contig,videobuf2_v4l2,v4l2_mem2mem,videobuf2_memops
mc                     81920  6 videodev,pisp_be,rpi_hevc_dec,videobuf2_v4l2,videobuf2_common,v4l2_mem2mem
gpio_keys              49152  0
i2c_brcmstb            49152  0
spi_bcm2835            49152  0
rp1_pio                65536  0
pwm_fan                49152  0
rp1_mailbox            49152  1
i2c_designware_platform    49152  0
spi_dw_mmio            49152  0
rp1_adc                49152  0
rp1_fw                 49152  1 rp1_pio
raspberrypi_gpiomem    49152  0
spi_dw                 49152  1 spi_dw_mmio
i2c_designware_core    49152  1 i2c_designware_platform
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
vc4                   425984  16
snd_soc_hdmi_codec     49152  2
snd_soc_core          311296  2 vc4,snd_soc_hdmi_codec
snd_compress           49152  1 snd_soc_core
snd_pcm_dmaengine      49152  1 snd_soc_core
v3d                   212992  8
snd_pcm               147456  4 snd_soc_hdmi_codec,snd_compress,snd_soc_core,snd_pcm_dmaengine
snd_timer              65536  1 snd_pcm
snd                   131072  9 snd_soc_hdmi_codec,snd_timer,snd_compress,snd_soc_core,snd_pcm
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
a0000@mcalab:~/Desktop/linux_driver/class/class_10 $ dmesg | tail -n 10
[    4.328660] NET: Registered PF_ALG protocol family
[    5.061577] macb 1f00100000.ethernet eth0: PHY [1f00100000.ethernet-ffffffff:01] driver [Broadcom BCM54213PE] (irq=POLL)
[    5.061586] macb 1f00100000.ethernet eth0: configuring for phy/rgmii-id link mode
[    5.064753] macb 1f00100000.ethernet: gem-ptp-timer ptp clock registered.
[    5.074408] brcmfmac: brcmf_cfg80211_set_power_mgmt: power save enabled
[    7.192671] Bluetooth: RFCOMM TTY layer initialized
[    7.192682] Bluetooth: RFCOMM socket layer initialized
[    7.192689] Bluetooth: RFCOMM ver 1.11
[ 3492.633679] newchrled: loading out-of-tree module taints kernel.
[ 3492.634314] newchrled_init
a0000@mcalab:~/Desktop/linux_driver/class/class_10 $ sudo rmmod newchrled
a0000@mcalab:~/Desktop/linux_driver/class/class_10 $ lsmod
Module                  Size  Used by
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
bluetooth             638976  31 hci_uart,btbcm,bnep,rfcomm
brcmfmac              376832  1 brcmfmac_wcc
brcmutil               49152  1 brcmfmac
cfg80211             1032192  1 brcmfmac
hid_multitouch         49152  0
aes_ce_blk             49152  4
rpi_hevc_dec           65536  0
pisp_be                49152  0
aes_ce_cipher          49152  1 aes_ce_blk
ghash_ce               49152  0
gf128mul               49152  1 ghash_ce
sha2_ce                49152  0
v4l2_mem2mem           65536  1 rpi_hevc_dec
ecdh_generic           49152  2 bluetooth
sha256_arm64           49152  1 sha2_ce
videobuf2_dma_contig    49152  2 pisp_be,rpi_hevc_dec
sha1_ce                49152  0
videobuf2_memops       49152  1 videobuf2_dma_contig
videobuf2_v4l2         49152  3 pisp_be,rpi_hevc_dec,v4l2_mem2mem
ecc                    65536  1 ecdh_generic
videodev              344064  4 pisp_be,rpi_hevc_dec,videobuf2_v4l2,v4l2_mem2mem
rfkill                 49152  6 bluetooth,cfg80211
sha1_generic           49152  1 sha1_ce
libaes                 49152  4 aes_ce_cipher,bluetooth,ghash_ce,aes_ce_blk
raspberrypi_hwmon      49152  0
videobuf2_common       98304  6 pisp_be,rpi_hevc_dec,videobuf2_dma_contig,videobuf2_v4l2,v4l2_mem2mem,videobuf2_memops
mc                     81920  6 videodev,pisp_be,rpi_hevc_dec,videobuf2_v4l2,videobuf2_common,v4l2_mem2mem
gpio_keys              49152  0
i2c_brcmstb            49152  0
spi_bcm2835            49152  0
rp1_pio                65536  0
pwm_fan                49152  0
rp1_mailbox            49152  1
i2c_designware_platform    49152  0
spi_dw_mmio            49152  0
rp1_adc                49152  0
rp1_fw                 49152  1 rp1_pio
raspberrypi_gpiomem    49152  0
spi_dw                 49152  1 spi_dw_mmio
i2c_designware_core    49152  1 i2c_designware_platform
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
vc4                   425984  16
snd_soc_hdmi_codec     49152  2
snd_soc_core          311296  2 vc4,snd_soc_hdmi_codec
snd_compress           49152  1 snd_soc_core
snd_pcm_dmaengine      49152  1 snd_soc_core
v3d                   212992  8
snd_pcm               147456  4 snd_soc_hdmi_codec,snd_compress,snd_soc_core,snd_pcm_dmaengine
snd_timer              65536  1 snd_pcm
snd                   131072  9 snd_soc_hdmi_codec,snd_timer,snd_compress,snd_soc_core,snd_pcm
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
a0000@mcalab:~/Desktop/linux_driver/class/class_10 $ dmesg | tail -n 10
[    5.061586] macb 1f00100000.ethernet eth0: configuring for phy/rgmii-id link mode
[    5.064753] macb 1f00100000.ethernet: gem-ptp-timer ptp clock registered.
[    5.074408] brcmfmac: brcmf_cfg80211_set_power_mgmt: power save enabled
[    7.192671] Bluetooth: RFCOMM TTY layer initialized
[    7.192682] Bluetooth: RFCOMM socket layer initialized
[    7.192689] Bluetooth: RFCOMM ver 1.11
[ 3492.633679] newchrled: loading out-of-tree module taints kernel.
[ 3492.634314] newchrled_init
[ 3492.634319] newchrled major = 509, minor = 0
[ 3732.676422] newchrled_exit
a0000@mcalab:~/Desktop/linux_driver/class/class_10 $ sudo modprobe newchrled
a0000@mcalab:~/Desktop/linux_driver/class/class_10 $ lsmod
Module                  Size  Used by
newchrled              49152  0
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
bluetooth             638976  31 hci_uart,btbcm,bnep,rfcomm
brcmfmac              376832  1 brcmfmac_wcc
brcmutil               49152  1 brcmfmac
cfg80211             1032192  1 brcmfmac
hid_multitouch         49152  0
aes_ce_blk             49152  4
rpi_hevc_dec           65536  0
pisp_be                49152  0
aes_ce_cipher          49152  1 aes_ce_blk
ghash_ce               49152  0
gf128mul               49152  1 ghash_ce
sha2_ce                49152  0
v4l2_mem2mem           65536  1 rpi_hevc_dec
ecdh_generic           49152  2 bluetooth
sha256_arm64           49152  1 sha2_ce
videobuf2_dma_contig    49152  2 pisp_be,rpi_hevc_dec
sha1_ce                49152  0
videobuf2_memops       49152  1 videobuf2_dma_contig
videobuf2_v4l2         49152  3 pisp_be,rpi_hevc_dec,v4l2_mem2mem
ecc                    65536  1 ecdh_generic
videodev              344064  4 pisp_be,rpi_hevc_dec,videobuf2_v4l2,v4l2_mem2mem
rfkill                 49152  6 bluetooth,cfg80211
sha1_generic           49152  1 sha1_ce
libaes                 49152  4 aes_ce_cipher,bluetooth,ghash_ce,aes_ce_blk
raspberrypi_hwmon      49152  0
videobuf2_common       98304  6 pisp_be,rpi_hevc_dec,videobuf2_dma_contig,videobuf2_v4l2,v4l2_mem2mem,videobuf2_memops
mc                     81920  6 videodev,pisp_be,rpi_hevc_dec,videobuf2_v4l2,videobuf2_common,v4l2_mem2mem
gpio_keys              49152  0
i2c_brcmstb            49152  0
spi_bcm2835            49152  0
rp1_pio                65536  0
pwm_fan                49152  0
rp1_mailbox            49152  1
i2c_designware_platform    49152  0
spi_dw_mmio            49152  0
rp1_adc                49152  0
rp1_fw                 49152  1 rp1_pio
raspberrypi_gpiomem    49152  0
spi_dw                 49152  1 spi_dw_mmio
i2c_designware_core    49152  1 i2c_designware_platform
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
vc4                   425984  16
snd_soc_hdmi_codec     49152  2
snd_soc_core          311296  2 vc4,snd_soc_hdmi_codec
snd_compress           49152  1 snd_soc_core
snd_pcm_dmaengine      49152  1 snd_soc_core
v3d                   212992  8
snd_pcm               147456  4 snd_soc_hdmi_codec,snd_compress,snd_soc_core,snd_pcm_dmaengine
snd_timer              65536  1 snd_pcm
snd                   131072  9 snd_soc_hdmi_codec,snd_timer,snd_compress,snd_soc_core,snd_pcm
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
a0000@mcalab:~/Desktop/linux_driver/class/class_10 $ dmesg | tail -n 10
[    5.064753] macb 1f00100000.ethernet: gem-ptp-timer ptp clock registered.
[    5.074408] brcmfmac: brcmf_cfg80211_set_power_mgmt: power save enabled
[    7.192671] Bluetooth: RFCOMM TTY layer initialized
[    7.192682] Bluetooth: RFCOMM socket layer initialized
[    7.192689] Bluetooth: RFCOMM ver 1.11
[ 3492.633679] newchrled: loading out-of-tree module taints kernel.
[ 3492.634314] newchrled_init
[ 3492.634319] newchrled major = 509, minor = 0
[ 3732.676422] newchrled_exit
[ 3761.043734] newchrled_init
a0000@mcalab:~/Desktop/linux_driver/class/class_10 $ cat /proc/devices
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
509 newchrled
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
a0000@mcalab:~/Desktop/linux_driver/class/class_10 $

```

---
Linux 裡面，如何用內核提供的 `cdev` 結構來註冊一個字元（character）設備，以及如何用 `cdev_init()` 來初始化它。

---

1. **`struct cdev` 定義**（位於 `include/linux/cdev.h`）

   ```c
   struct cdev {
       struct kobject             kobj;
       struct module             *owner;
       const struct file_operations *ops;   // 字符設備的操作函式集合
       struct list_head           list;
       dev_t                      dev;     // 設備號（major/minor）
       unsigned int               count;   // 連續設備號個數
   };
   ```

   * `ops`：指向一張 `file_operations` 結構，裡面放你的 open/read/write/… 等回調函式。
   * `dev`：用來存放這個字元設備的主／從設備號。

   在真正註冊前，你要先在程式裡宣告一個變數，例如

   ```c
   struct cdev test_cdev;
   ```

2. **`cdev_init()` 函式**

   ```c
   void cdev_init(struct cdev *cdev, const struct file_operations *fops);
   ```

   * **參數**

     * `cdev`：你剛宣告的那個 `struct cdev` 變數指標。
     * `fops`：你的 `file_operations` 結構指標，裡面包含打算綁定到此設備的所有操作函式。

   呼叫完 `cdev_init(&test_cdev, &my_fops);` 之後，內部就會把 `test_cdev.ops = &my_fops;`，並完成其他基礎設定，接下來再配合 `cdev_add()` 將它真正登記到系統中。

---

簡而言之，這裡重點在教你

1. 用 `struct cdev` 表示一個字元設備的「物件」；
2. 用 `cdev_init()` 把你的操作函式表 `file_operations` 綁到那個物件上；
3. 再用 `cdev_add()`（截圖沒提，但後續會用到）把它加到系統裡。


---

**`cdev_init()`** 的示例程式碼，步驟如下：

1. **宣告一個字元設備物件**

   ```c
   struct cdev testcdev;
   ```

2. **定義對應的 file\_operations**

   ```c
   static struct file_operations test_fops = {
       .owner = THIS_MODULE,
       /* 其他 open/read/write… 等回調函式 */
   };
   ```

3. **設定 cdev 的 owner**

   ```c
   testcdev.owner = THIS_MODULE;
   ```

4. **呼叫 `cdev_init()` 來綁定 ops、初始化 cdev**

   ```c
   cdev_init(&testcdev, &test_fops);  /* 將 test_fops 設入 testcdev.ops 並做基本初始化 */
   ```

完成以上，就把你的 `file_operations` 表綁到 `testcdev` 這個字元設備物件上，接下來就可以用 `cdev_add()` 把它註冊到系統中了。

---
這段文字是在講 **`cdev_add()`** 這個函式的用途和原型：

1. 先前已經用 `cdev_init()` 把你的 `struct cdev` 變數初始化好，接下來就要呼叫 `cdev_add()`，把這個字元設備「加」到 Linux 核心裡去，讓核心開始管理它。

2. 它還貼出了 `cdev_add()` 的函式原型，大致長這樣：

   ```c
   int cdev_add(struct cdev *p, dev_t dev, unsigned count);
   ```

   * **`p`**：你要註冊的那個 `cdev` 結構體指標
   * **`dev`**：起始的設備號（`dev_t`，包含 major/minor）
   * **`count`**：要註冊多少個連續的次設備號

下面還標明這段內容出自「I.MX6U 嵌入式 Linux 驅動開發指南」——就是在教你完整的三步驟：

1. `struct cdev testcdev;`
2. `cdev_init(&testcdev, &my_fops);`
3. `cdev_add(&testcdev, dev, 1);`   將它註冊到系統裡。

---

這段程式碼就是字元設備註冊的「反向操作」——**移除**（deregister）一個之前用 `cdev_add()` 加入過的 `struct cdev`。

```c
void cdev_del(struct cdev *p)
{
    cdev_unmap(p->dev, p->count);
    kobject_put(&p->kobj);
}
```

1. **`cdev_unmap(p->dev, p->count);`**
   這行會把先前由 `cdev_add()` 在核心中為 `p->dev`（起始設備號）到 `p->dev + p->count - 1` 這一段號碼分配的映射（mapping）給撤掉，讓系統不再把這些設備號交給你的驅動。

2. **`kobject_put(&p->kobj);`**
   `struct cdev` 中內嵌了一個 `kobject`（核心物件），當你初始化並加入時，`kobject` 的 reference count 會加 1；`kobject_put()` 則是把這個計數減 1，當降到 0 時就真正釋放掉這個 `cdev` 在 sysfs 中對應的那個物件節點。

---

**總結**：呼叫 `cdev_del(&my_cdev);` 就等於把你之前 `cdev_add(&my_cdev, dev, count);` 所做的映射跟 kobject 都撤銷、釋放，讓這個字元設備回到「未註冊」的狀態。
