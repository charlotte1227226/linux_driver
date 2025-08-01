# 自動創建設備節點
1. 以前的實驗中，都需要手動調用`mknod`創建設備節點。為此 2.6 內核引入了`udev`機制，替換`devfs`。`udev`機制提供熱插拔管理，可以在加載驅動的時候自動創建`/dev/xx`設備文件。busybox提供了`udev`的簡化版本
- 樹梅派沒有`mdev`


---
# 終端機結果
```bash
a0000@mcalab:~/Desktop/linux_driver/class/class_11 $ sudo cp newchrled.ko /lib/modules/$(uname -r)/kernel/drivers/misc/
a0000@mcalab:~/Desktop/linux_driver/class/class_11 $ ls /lib/modules/$(uname -r)/kernel/drivers/misc/
bcm2835_smi.ko.xz  chrdevbase.ko  eeprom  led.ko  newchrled.ko  rp1-pio.ko.xz  rpi_led.ko  ti-st  ws2812-pio-rp1.ko.xz
a0000@mcalab:~/Desktop/linux_driver/class/class_11 $ lsmod
Module                  Size  Used by
rfcomm                 81920  4
algif_hash             49152  1
algif_skcipher         49152  1
af_alg                 49152  6 algif_hash,algif_skcipher
bnep                   49152  2
binfmt_misc            49152  1
brcmfmac_wcc           49152  0
brcmfmac              376832  1 brcmfmac_wcc
spidev                 49152  0
hci_uart               65536  0
brcmutil               49152  1 brcmfmac
btbcm                  49152  1 hci_uart
aes_ce_blk             49152  4
aes_ce_cipher          49152  1 aes_ce_blk
cfg80211             1032192  1 brcmfmac
ghash_ce               49152  0
bluetooth             638976  31 hci_uart,btbcm,bnep,rfcomm
gf128mul               49152  1 ghash_ce
hid_multitouch         49152  0
pisp_be                49152  0
sha2_ce                49152  0
rpi_hevc_dec           65536  0
sha256_arm64           49152  1 sha2_ce
sha1_ce                49152  0
v4l2_mem2mem           65536  1 rpi_hevc_dec
ecdh_generic           49152  2 bluetooth
videobuf2_dma_contig    49152  2 pisp_be,rpi_hevc_dec
ecc                    65536  1 ecdh_generic
videobuf2_memops       49152  1 videobuf2_dma_contig
rfkill                 49152  6 bluetooth,cfg80211
videobuf2_v4l2         49152  3 pisp_be,rpi_hevc_dec,v4l2_mem2mem
sha1_generic           49152  1 sha1_ce
libaes                 49152  4 aes_ce_cipher,bluetooth,ghash_ce,aes_ce_blk
videodev              344064  4 pisp_be,rpi_hevc_dec,videobuf2_v4l2,v4l2_mem2mem
videobuf2_common       98304  6 pisp_be,rpi_hevc_dec,videobuf2_dma_contig,videobuf2_v4l2,v4l2_mem2mem,videobuf2_memops
raspberrypi_hwmon      49152  0
mc                     81920  6 videodev,pisp_be,rpi_hevc_dec,videobuf2_v4l2,videobuf2_common,v4l2_mem2mem
spi_bcm2835            49152  0
rp1_pio                65536  0
gpio_keys              49152  0
i2c_brcmstb            49152  0
i2c_designware_platform    49152  0
spi_dw_mmio            49152  0
rp1_adc                49152  0
pwm_fan                49152  0
rp1_fw                 49152  1 rp1_pio
raspberrypi_gpiomem    49152  0
spi_dw                 49152  1 spi_dw_mmio
nvmem_rmem             49152  0
rp1_mailbox            49152  1
i2c_designware_core    49152  1 i2c_designware_platform
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
drm_display_helper     49152  1 vc4
drm_dma_helper         49152  2 vc4
v3d                   212992  8
drm_shmem_helper       49152  1 v3d
cec                    65536  1 vc4
drm_kms_helper        245760  3 drm_dma_helper,vc4,drm_shmem_helper
gpu_sched              98304  1 v3d
drm                   688128  20 gpu_sched,drm_kms_helper,drm_dma_helper,v3d,vc4,drm_shmem_helper,drm_display_helper
drm_panel_orientation_quirks    65536  1 drm
backlight              49152  2 drm_kms_helper,drm
uio_pdrv_genirq        49152  0
uio                    49152  1 uio_pdrv_genirq
a0000@mcalab:~/Desktop/linux_driver/class/class_11 $ ls /dev
autofs           disk        gpiochip12  hidraw2  kvm    loop-control  net   ram1   ram5     shm         tty1   tty19  tty28  tty37  tty46  tty55  tty7       usb     vcs6   vcsu         vhost-net    video27   watchdog0
block            dma_heap    gpiochip13  hidraw3  log    mapper        null  ram10  ram6     snd         tty10  tty2   tty29  tty38  tty47  tty56  tty8       v4l     vcs7   vcsu1        vhost-vsock  video28   zero
btrfs-control    dri         gpiochip4   hidraw4  loop0  media0        pio0  ram11  ram7     spidev0.0   tty11  tty20  tty3   tty39  tty48  tty57  tty9       vcio    vcsa   vcsu2        video19      video29
bus              fb0         gpiomem0    hwrng    loop1  media1        port  ram12  ram8     spidev0.1   tty12  tty21  tty30  tty4   tty49  tty58  ttyAMA0    vc-mem  vcsa1  vcsu3        video20      video30
cec0             fd          gpiomem1    i2c-1    loop2  media2        ppp   ram13  ram9     spidev10.0  tty13  tty22  tty31  tty40  tty5   tty59  ttyAMA10   vcs     vcsa2  vcsu4        video21      video31
cec1             full        gpiomem2    i2c-13   loop3  mem           pps0  ram14  random   stderr      tty14  tty23  tty32  tty41  tty50  tty6   ttyprintk  vcs1    vcsa3  vcsu5        video22      video32
char             fuse        gpiomem3    i2c-14   loop4  mmcblk0       ptmx  ram15  rfkill   stdin       tty15  tty24  tty33  tty42  tty51  tty60  udmabuf    vcs2    vcsa4  vcsu6        video23      video33
console          gpiochip0   gpiomem4    initctl  loop5  mmcblk0p1     ptp0  ram2   rtc      stdout      tty16  tty25  tty34  tty43  tty52  tty61  uhid       vcs3    vcsa5  vcsu7        video24      video34
cpu_dma_latency  gpiochip10  hidraw0     input    loop6  mmcblk0p2     pts   ram3   rtc0     tty         tty17  tty26  tty35  tty44  tty53  tty62  uinput     vcs4    vcsa6  vga_arbiter  video25      video35
cuse             gpiochip11  hidraw1     kmsg     loop7  mqueue        ram0  ram4   serial0  tty0        tty18  tty27  tty36  tty45  tty54  tty63  urandom    vcs5    vcsa7  vhci         video26      watchdog
a0000@mcalab:~/Desktop/linux_driver/class/class_11 $ sudo depmod -a
a0000@mcalab:~/Desktop/linux_driver/class/class_11 $ sudo modprobe newchrled
a0000@mcalab:~/Desktop/linux_driver/class/class_11 $ ls /dev/newchrled -l
crw------- 1 root root 509, 0 Aug  2 00:21 /dev/newchrled
a0000@mcalab:~/Desktop/linux_driver/class/class_11 $ dmesg | tail -n 10
[    5.296297] macb 1f00100000.ethernet: gem-ptp-timer ptp clock registered.
[    5.306596] brcmfmac: brcmf_cfg80211_set_power_mgmt: power save enabled
[    7.449980] Bluetooth: RFCOMM TTY layer initialized
[    7.449991] Bluetooth: RFCOMM socket layer initialized
[    7.449999] Bluetooth: RFCOMM ver 1.11
[  469.173131] logitech-hidpp-device 0003:046D:4091.0005: HID++ 4.5 device connected.
[  492.429139] warning: `iwconfig' uses wireless extensions which will stop working for Wi-Fi 7 hardware; use nl80211
[ 4408.208670] newchrled: loading out-of-tree module taints kernel.
[ 4408.209224] newchrled_init
[ 4408.209228] newchrled major = 509, minor = 0
a0000@mcalab:~/Desktop/linux_driver/class/class_11 $ rmmod newchrled 
rmmod: ERROR: ../libkmod/libkmod-module.c:856 kmod_module_remove_module() could not remove 'newchrled': Operation not permitted
rmmod: ERROR: could not remove module newchrled: Operation not permitted
a0000@mcalab:~/Desktop/linux_driver/class/class_11 $ sudo rmmod newchrled 
a0000@mcalab:~/Desktop/linux_driver/class/class_11 $ dmesg | tail -n 10
[    5.306596] brcmfmac: brcmf_cfg80211_set_power_mgmt: power save enabled
[    7.449980] Bluetooth: RFCOMM TTY layer initialized
[    7.449991] Bluetooth: RFCOMM socket layer initialized
[    7.449999] Bluetooth: RFCOMM ver 1.11
[  469.173131] logitech-hidpp-device 0003:046D:4091.0005: HID++ 4.5 device connected.
[  492.429139] warning: `iwconfig' uses wireless extensions which will stop working for Wi-Fi 7 hardware; use nl80211
[ 4408.208670] newchrled: loading out-of-tree module taints kernel.
[ 4408.209224] newchrled_init
[ 4408.209228] newchrled major = 509, minor = 0
[ 4512.346793] newchrled_exit
a0000@mcalab:~/Desktop/linux_driver/class/class_11 $ ls /dev/newchrled -l
ls: cannot access '/dev/newchrled': No such file or directory
a0000@mcalab:~/Desktop/linux_driver/class/class_11 $ ls /dev
autofs           disk        gpiochip12  hidraw2  kvm    loop-control  net   ram1   ram5     shm         tty1   tty19  tty28  tty37  tty46  tty55  tty7       usb     vcs6   vcsu         vhost-net    video27   watchdog0
block            dma_heap    gpiochip13  hidraw3  log    mapper        null  ram10  ram6     snd         tty10  tty2   tty29  tty38  tty47  tty56  tty8       v4l     vcs7   vcsu1        vhost-vsock  video28   zero
btrfs-control    dri         gpiochip4   hidraw4  loop0  media0        pio0  ram11  ram7     spidev0.0   tty11  tty20  tty3   tty39  tty48  tty57  tty9       vcio    vcsa   vcsu2        video19      video29
bus              fb0         gpiomem0    hwrng    loop1  media1        port  ram12  ram8     spidev0.1   tty12  tty21  tty30  tty4   tty49  tty58  ttyAMA0    vc-mem  vcsa1  vcsu3        video20      video30
cec0             fd          gpiomem1    i2c-1    loop2  media2        ppp   ram13  ram9     spidev10.0  tty13  tty22  tty31  tty40  tty5   tty59  ttyAMA10   vcs     vcsa2  vcsu4        video21      video31
cec1             full        gpiomem2    i2c-13   loop3  mem           pps0  ram14  random   stderr      tty14  tty23  tty32  tty41  tty50  tty6   ttyprintk  vcs1    vcsa3  vcsu5        video22      video32
char             fuse        gpiomem3    i2c-14   loop4  mmcblk0       ptmx  ram15  rfkill   stdin       tty15  tty24  tty33  tty42  tty51  tty60  udmabuf    vcs2    vcsa4  vcsu6        video23      video33
console          gpiochip0   gpiomem4    initctl  loop5  mmcblk0p1     ptp0  ram2   rtc      stdout      tty16  tty25  tty34  tty43  tty52  tty61  uhid       vcs3    vcsa5  vcsu7        video24      video34
cpu_dma_latency  gpiochip10  hidraw0     input    loop6  mmcblk0p2     pts   ram3   rtc0     tty         tty17  tty26  tty35  tty44  tty53  tty62  uinput     vcs4    vcsa6  vga_arbiter  video25      video35
cuse             gpiochip11  hidraw1     kmsg     loop7  mqueue        ram0  ram4   serial0  tty0        tty18  tty27  tty36  tty45  tty54  tty63  urandom    vcs5    vcsa7  vhci         video26      watchdog
a0000@mcalab:~/Desktop/linux_driver/class/class_11 $ 
```

---
「自動建立／刪除 Linux 裝置節點」的機制，主要內容可以分成以下幾點：

1. **為什麼要自動建節點？**
   傳統上我們在載入一個驅動程式（`modprobe xxx`）之後，還得手動執行

   ```bash
   mknod /dev/your_device c <major> <minor>
   ```

   才能在 `/dev` 下看到對應的裝置文件。這樣既麻煩又容易弄錯。

2. **udev 機制**

   * `udev` 是 Linux 上的一個使用者空間（user-space）程式，專門負責監聽 kernel 發出的「新增／移除裝置」事件（uevent），然後自動在 `/dev` 底下建立或刪除對應的節點檔案。
   * 當你用 `modprobe` 載入驅動、kernel 探測到硬體之後，`udev` 就會在 `/dev` 下幫你做出相應的裝置檔。
   * 同理，用 `rmmod` 卸載驅動時，`udev` 也會自動把對應的 `/dev` 檔案刪掉。

    - 熱插拔:隨時加載卸載的意思
3. **mdev（busybox 版 udev）**

   * 在嵌入式系統中，我們常用 busybox 來打造小巧的 rootfs。busybox 內建了一個簡化版的 udev，叫做 `mdev`。
   * `mdev` 功能和 udev 類似，但更精簡，適合資源有限的環境。
   * 在嵌入式 Linux 裡，你就用 `mdev` 來自動管理 `/dev` 底下的裝置節點。

---

**總結**：這段文字在解釋，透過 `udev`（或在嵌入式用的 `mdev`）機制，當你用 `modprobe` 載入／`rmmod` 卸載驅動時，就能自動在 `/dev` 底下建立或移除對應的裝置節點，不再需要手動 `mknod`。

---

這裡的 **`class`** 並不是 C++ 的「類別」，而是 **Linux device model（裝置模型）** 裡的一個核心結構──**`struct class`**。

---
# class
## 1. 在 kernel 裡，`struct class` 是什麼？

* 它代表一「組同類型的裝置」（device class），並在 **sysfs** 下形成一個目錄 `/sys/class/<name>`。
* 比方說：

  * `class_create(…, "net")` 就對應到 `/sys/class/net`（網路裝置）
  * `class_create(…, "block")` 就對應到 `/sys/class/block`（塊設備）
* 每個 class 底下，你可以再用 `device_create()` 把具體的裝置（struct device）掛到這個 class 上，這樣 userspace（udev/devtmpfs）就會幫你在 `/dev` 自動產生對應的節點。

---

## 2. 為什麼要用 class？

1. **分類管理**

   * 把相同種類、共用屬性或 sysfs attribute 的裝置，集中在同一個目錄下管理。
2. **自動創建 `/dev`**

   * 有了 class + device，devtmpfs/udev 就能自動對應建立 `/dev/<name><N>`，而不用你手動 `mknod`。
3. **統一 sysfs 接口**

   * 你可以在 class 層級定義一組屬性（attribute group），所有屬於此 class 的裝置都能共享這些屬性檔。

---

## 3. 典型流程範例

```c
#include <linux/device.h>
#include <linux/fs.h>

static struct class *foo_class;
static dev_t foo_dev;

// module init
static int __init foo_init(void)
{
    /* 1) 分配一個 major/minor */
    alloc_chrdev_region(&foo_dev, 0, 1, "foo");

    /* 2) 建立一個 class，對應 /sys/class/foo */
    foo_class = class_create(THIS_MODULE, "foo");

    /* 3) 建立一個 device，這會在 /dev/foo0 產生節點 */
    device_create(foo_class, NULL, foo_dev, NULL, "foo%d", 0);

    return 0;
}

/* module exit */
static void __exit foo_exit(void)
{
    /* 移除 device 與 class，並釋放號碼 */
    device_destroy(foo_class, foo_dev);
    class_destroy(foo_class);
    unregister_chrdev_region(foo_dev, 1);
}

module_init(foo_init);
module_exit(foo_exit);
```

* **`class_create`** → 在 sysfs 建立 `/sys/class/foo`
* **`device_create`** → 在 `/sys/class/foo/foo0` 下新增一個 device 物件，並觸發 udev/devtmpfs 幫你在 `/dev/foo0` 建節點
* **`class_destroy`** + **`device_destroy`** → 清除相應的 sysfs entry 與 `/dev` 裡的節點

---

### 總結

* **`struct class`**：Linux 裝置模型裡做「一群相同類型裝置」的容器
* **搭配 sysfs + udev/devtmpfs**：能實現自動在 `/dev` 產生裝置節點、統一管理屬性
* **使用方式**：呼叫 `class_create()`→`device_create()`，退場時再呼 `device_destroy()`→`class_destroy()`。

---
`class_create` 這個巨集只帶 **兩個**參數：

```c
struct class *foo = class_create(owner, name);
```

1. **`owner`** (`struct module *`)

   * 表示「誰」擁有（own）這個 class，kernel 會把它存在 `struct class` 裡，並對應地做 module use‐count。
   * 通常就是傳 `THIS_MODULE`，代表目前在初始化／載入的這個模組。
   * 作用：確保在這個 class 還被「擁有」時，module 不會被意外卸載。

2. **`name`** (`const char *`)

   * 這個 class 在 sysfs（以及 `/sys/class`）底下的目錄名稱。
   * 例如傳 `"foo"`，就會建立 `/sys/class/foo`，後續你再對這個 class 呼叫 `device_create(...)`，就會出現在 `/sys/class/foo/<device 名稱>`。

---

### 底層實際呼叫

巨集展開後等同：

```c
({
    static struct lock_class_key __key;
    __class_create(owner, name, &__key);
})
```

* `__class_create` 真正的函式簽名是：

  ```c
  struct class * __class_create(
      struct module *owner,
      const char    *name,
      struct lock_class_key *key
  );
  ```

* `&__key` 則是用來做 lock‐dependency 驗證用的「私有金鑰」，你不需要理會它的細節。

---

**範例**：

```c
static struct class *my_class;

static int __init my_init(void)
{
    /* 建立一個名為 "mydev" 的 class，module 擁有者用 THIS_MODULE */
    my_class = class_create(THIS_MODULE, "mydev");
    if (IS_ERR(my_class))
        return PTR_ERR(my_class);
    /* … 接著可以呼叫 device_create(my_class, …) … */
    return 0;
}
```

---
`class_destroy` 是與 `class_create` 相對應，用來**銷毀（unregister）先前建立好的 device class**，並清理相關資源和 sysfs 目錄的 API。

---

## 概念

1. **移除 sysfs entry**

   * 將 `/sys/class/<name>` 這個目錄刪除。
2. **通知 devtmpfs/udev**

   * 讓使用者空間的 hotplug 管理程式去移除 `/dev` 底下對應的節點。
3. **釋放 kernel 資源**

   * 釋放在 `__class_create()`（或 `class_create` 巨集）裡分配的記憶體與結構。
4. **搭配 module exit**

   * 通常在 driver 卸載路徑（`module_exit`）裡呼叫，確保載入時建立的東西能對稱地被清理掉。

---

## 原型 & 參數

```c
void class_destroy(struct class *cls);
```

|   參數  | 型態               | 說明                                                  |
| :---: | :--------------- | :-------------------------------------------------- |
| `cls` | `struct class *` | 由 `class_create(owner, name)` 回傳的指標，指定要銷毀的那個 class。 |

* **回傳值**：無（`void`）。
* **呼叫時機**：在 module 的退出函式裡，或任何需要卸載該 class 的時候。

---

### 範例

```c
static struct class *foo_class;

static int __init foo_init(void)
{
    foo_class = class_create(THIS_MODULE, "foo");
    if (IS_ERR(foo_class))
        return PTR_ERR(foo_class);
    /* … 其他初始化 … */
    return 0;
}

static void __exit foo_exit(void)
{
    /* 必須先移除所有 device，再銷毀 class */
    class_destroy(foo_class);
}

module_init(foo_init);
module_exit(foo_exit);
```

如此即可在驅動卸載時，完整地將先前建立的 `/sys/class/foo` 以及對應的 `/dev` 節點統一清除。

---
在這裡看到的那個省略號（`...`）就是 C 語言的 **可變參數**（variadic arguments）語法，意思是這個函式除了前面固定的參數以外，還可以像 `printf` 一樣再傳一串「對應 `fmt` 字符串」的額外參數。

---

## `device_create` 原型回顧

```c
struct device *device_create(
    struct class     *class,    /* 要掛在哪個 class 底下 */
    struct device    *parent,   /* 父裝置（一般傳 NULL） */
    dev_t              devt,     /* 編號：major/minor */
    void              *drvdata, /* driver 私有資料（可傳 NULL） */
    const char        *fmt,      /* 裝置名稱格式字串 */
    ...                         /* ← 可變參數，跟 fmt 對應 */
);
```

* **`fmt`**：類似 `printf` 那樣的格式字串，用來指定 `/dev` 底下你要建立的節點名稱，例如 `"foo%d"`、`"ttyUSB%d"` 等等。
* **`...`**：後面緊跟的參數要跟 `fmt` 裡的 `%` 佔位符吻合，函式內部會呼叫 `vsnprintf()` 等 API 生成最終的名字。

---

### 常見範例

```c
/* 假設 foo_class 是先前用 class_create 建好的 struct class * */
dev_t foo_dev = MKDEV(240, 0);

/* 建立一支叫做 /dev/foo0 的裝置節點 */
struct device *dev = device_create(
    foo_class,    /* class */
    NULL,         /* parent */
    foo_dev,      /* devt */
    NULL,         /* drvdata */
    "foo%d",      /* fmt 字符串 */
    0             /* fmt 裡的 %d=0 */
);

/* 結果：kernel 會在 /sys/class/foo/foo0 以及 /dev/foo0 自動建立對應節點 */
```

---

### 為什麼要這樣做？

* **靈活命名**：如果你要一次性產生多個同類型的裝置（foo0、foo1、foo2 …），只要在 `fmt` 裡用 `%d`，然後在 `...` 裡給不同的整數就好。
* **簡化程式碼**：不用手動拼字串、`sprintf` 再傳給函式，`device_create` 已經把這件事情包好了。
* **自動化 `/dev`**：呼叫之後，devtmpfs/udev 就會接到事件，自動在 `/dev` 底下建立或移除裝置節點。

---

`device_destroy` 跟 `device_create` 是一對操作：前者用來銷毀（unregister）你先前用 `device_create` 建立的那支裝置，並通知 sysfs/devtmpfs（或 udev）移除對應的 `/dev` 節點。

---

## 概念

1. **移除 sysfs entry**

   * 刪除 `/sys/class/<your_class>/<device_name>` 這個目錄。
2. **通知 devtmpfs/udev**

   * 讓使用者空間的 hotplug 管理程式把 `/dev/<device_name>` 也同時移除。
3. **釋放 kernel 資源**

   * 對應 `device_create` 分配的 `struct device` 物件會被清理。

---

## 原型 & 參數

```c
#include <linux/device.h>

void device_destroy(
    struct class *cls,  /* 與哪個 class 關聯 */
    dev_t         devt  /* 編號：同 create 時用的 major/minor */
);
```

|   參數   | 型態               | 說明                                 |
| :----: | :--------------- | :--------------------------------- |
|  `cls` | `struct class *` | `device_create` 時傳入的 class 物件指標    |
| `devt` | `dev_t`          | `device_create` 時用的 major/minor 編號 |

---

## 範例流程

```c
static struct class *foo_class;
static dev_t          foo_dev;

static int __init foo_init(void)
{
    /* 1) 先建立 class */
    foo_class = class_create(THIS_MODULE, "foo");
    if (IS_ERR(foo_class))
        return PTR_ERR(foo_class);

    /* 2) 分配一組 major/minor */
    alloc_chrdev_region(&foo_dev, 0, 1, "foo");

    /* 3) 建立 /dev/foo0 */
    device_create(foo_class, NULL, foo_dev, NULL, "foo%d", 0);
    return 0;
}

static void __exit foo_exit(void)
{
    /* 卸載時：先移除 device，再移除 class */
    device_destroy(foo_class, foo_dev);
    class_destroy(foo_class);
    unregister_chrdev_region(foo_dev, 1);
}

module_init(foo_init);
module_exit(foo_exit);
```

* `device_destroy(foo_class, foo_dev)` 會對應移除：

  * `/sys/class/foo/foo0`
  * `/dev/foo0`

並且清除內部 `struct device` 結構。

---
這個編譯錯誤其實在告訴你：**你呼叫的 `class_create(THIS_MODULE, NEWCHRLED_NAME)` 已經跟目前 kernel 裡的簽名 (signature) 不符**，它現在只接受一個參數──類名（`const char *name`），而不再需要傳入 `struct module *owner`。詳細原因如下：

* **舊版 API**（書上範例）

  ```c
  /* 兩個參數：owner + name */
  #define class_create(owner, name)  \
  ({                                  \
      static struct lock_class_key __key;   \
      __class_create(owner, name, &__key);  \
  })
  ```
* **新版簽名**（Linux ≥6.4 已移除 owner 參數）

  ```c
  extern struct class * __must_check class_create(const char *name);
  ```

  因此你傳入的第一個參數 `THIS_MODULE`（`struct module *`）會被視為「指向字串的指標」──型別完全不符，編譯器就報

  ```
  passing argument 1 of ‘class_create’ from incompatible pointer type
  too many arguments to function ‘class_create’
  ```

  ([GitLab][1])

---

## 怎麼改？

把呼叫改成 **只帶類名** 就能通過編譯，例如：

```diff
- newchrled.class = class_create(THIS_MODULE, NEWCHRLED_NAME);
+ newchrled.class = class_create(NEWCHRLED_NAME);
```

如果真的想自己指定 module owner（通常不需要），可以直接呼叫底層的 `__class_create`：

```c
static struct lock_class_key __key;
newchrled.class = __class_create(THIS_MODULE, NEWCHRLED_NAME, &__key);
```

但在多數 modern kernel 裡，只要 `class_create("foo")`，owner 會由框架自動處理。

[1]: https://git.duagon.com/project/13/13mdis/linux/13MD05-90/-/issues/364?utm_source=chatgpt.com "Prepare MDIS to be built against kernel 6.6 - Explore projects - duagon"
