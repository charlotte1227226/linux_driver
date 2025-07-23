# 應用程序的編寫
linux下一切都是文件，首先要 `open`，(透過`man 2 open`裡查看要調用什麼函示庫，在樹梅派上`open`要用`#include <fcntl.h>`)
# 測試
1. 加載驅動`modprobe`
   - `modprobe chrdevbase.ko`
2. 進入/dev查看設備文件，名字為驅動中註冊字符設備的時候設置的名字，但是實際沒有因為我們沒有創建設備節點。
3. 手動創建設備節點 `mknod /dev/chrdevbase c 200 0`
   - `mknod` : 創建節點
   - `c` : 字符設備
   - `200` : 主設備號
   - `0` : 次設備號
4. 測試
# chrdevbase 虛擬設備驅動對的編寫


---
## 終端機結果
```bash
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ gcc ./chrdevbaseAPP.c -o chrdevbaseAPP
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ file ./chrdevbaseAPP
./chrdevbaseAPP: ELF 64-bit LSB pie executable, ARM aarch64, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux-aarch64.so.1, BuildID[sha1]=ca1e1e861ce4c163b7f3ba64e092585e40ffdae2, for GNU/Linux 3.7.0, not stripped
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ uname -m
aarch64
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ 
```
```bash
a0000@mcalab:~/Desktop $ cd ./linux_driver/class/class_4
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ make
make -C /lib/modules/6.12.25+rpt-rpi-2712/build M=/home/a0000/Desktop/linux_driver/class/class_4 modules
make[1]: Entering directory '/home/a0000/kernel-src/linux'
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_4/chrdevbase.o
  MODPOST /home/a0000/Desktop/linux_driver/class/class_4/Module.symvers
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_4/chrdevbase.mod.o
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_4/.module-common.o
  LD [M]  /home/a0000/Desktop/linux_driver/class/class_4/chrdevbase.ko
make[1]: Leaving directory '/home/a0000/kernel-src/linux'
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ sudo cp chrdevbase.ko /lib/modules/$(uname -r)/kernel/drivers/misc/
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ ls /lib/modules/$(uname -r)/kernel/drivers/misc/
bcm2835_smi.ko.xz  chrdevbase.ko  eeprom  rp1-pio.ko.xz  ti-st  ws2812-pio-rp1.ko.xz
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ modeprobe chrdevbase
bash: modeprobe: command not found
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ modprobe chrdevbase
modprobe: ERROR: could not insert 'chrdevbase': Operation not permitted
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ sudo modprobe chrdevbase
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ lsmod
Module                  Size  Used by
chrdevbase             49152  0
rfcomm                 81920  4
algif_hash             49152  1
algif_skcipher         49152  1
af_alg                 49152  6 algif_hash,algif_skcipher
bnep                   49152  2
binfmt_misc            49152  1
spidev                 49152  0
brcmfmac_wcc           49152  0
hci_uart               65536  0
btbcm                  49152  1 hci_uart
brcmfmac              376832  1 brcmfmac_wcc
brcmutil               49152  1 brcmfmac
bluetooth             638976  31 hci_uart,btbcm,bnep,rfcomm
rpi_hevc_dec           65536  0
cfg80211             1032192  1 brcmfmac
aes_ce_blk             49152  4
pisp_be                49152  0
aes_ce_cipher          49152  1 aes_ce_blk
v4l2_mem2mem           65536  1 rpi_hevc_dec
ghash_ce               49152  0
gf128mul               49152  1 ghash_ce
sha2_ce                49152  0
sha256_arm64           49152  1 sha2_ce
videobuf2_dma_contig    49152  2 pisp_be,rpi_hevc_dec
hid_multitouch         49152  0
videobuf2_memops       49152  1 videobuf2_dma_contig
sha1_ce                49152  0
videobuf2_v4l2         49152  3 pisp_be,rpi_hevc_dec,v4l2_mem2mem
videodev              344064  4 pisp_be,rpi_hevc_dec,videobuf2_v4l2,v4l2_mem2mem
ecdh_generic           49152  2 bluetooth
ecc                    65536  1 ecdh_generic
sha1_generic           49152  1 sha1_ce
rfkill                 49152  6 bluetooth,cfg80211
libaes                 49152  4 aes_ce_cipher,bluetooth,ghash_ce,aes_ce_blk
videobuf2_common       98304  6 pisp_be,rpi_hevc_dec,videobuf2_dma_contig,videobuf2_v4l2,v4l2_mem2mem,videobuf2_memops
raspberrypi_hwmon      49152  0
mc                     81920  6 videodev,pisp_be,rpi_hevc_dec,videobuf2_v4l2,videobuf2_common,v4l2_mem2mem
rp1_pio                65536  0
gpio_keys              49152  0
i2c_brcmstb            49152  0
spi_bcm2835            49152  0
rp1_mailbox            49152  1
spi_dw_mmio            49152  0
rp1_adc                49152  0
pwm_fan                49152  0
rp1_fw                 49152  1 rp1_pio
raspberrypi_gpiomem    49152  0
spi_dw                 49152  1 spi_dw_mmio
i2c_designware_platform    49152  0
nvmem_rmem             49152  0
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
vc4                   425984  16
snd_soc_hdmi_codec     49152  2
snd_soc_core          311296  2 vc4,snd_soc_hdmi_codec
snd_compress           49152  1 snd_soc_core
snd_pcm_dmaengine      49152  1 snd_soc_core
snd_pcm               147456  4 snd_soc_hdmi_codec,snd_compress,snd_soc_core,snd_pcm_dmaengine
snd_timer              65536  1 snd_pcm
snd                   131072  9 snd_soc_hdmi_codec,snd_timer,snd_compress,snd_soc_core,snd_pcm
drm_display_helper     49152  1 vc4
v3d                   212992  8
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
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ dmesg | tail -n 10
[    4.617577] NET: Registered PF_ALG protocol family
[    5.346647] macb 1f00100000.ethernet eth0: PHY [1f00100000.ethernet-ffffffff:01] driver [Broadcom BCM54213PE] (irq=POLL)
[    5.346656] macb 1f00100000.ethernet eth0: configuring for phy/rgmii-id link mode
[    5.349779] macb 1f00100000.ethernet: gem-ptp-timer ptp clock registered.
[    5.360707] brcmfmac: brcmf_cfg80211_set_power_mgmt: power save enabled
[    7.462068] Bluetooth: RFCOMM TTY layer initialized
[    7.462080] Bluetooth: RFCOMM socket layer initialized
[    7.462087] Bluetooth: RFCOMM ver 1.11
[ 6490.564389] chrdevbase: loading out-of-tree module taints kernel.
[ 6490.564837] chrdevbase_init
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ cd /
a0000@mcalab:/ $ ls
bin  boot  dev  etc  home  lib  lost+found  media  mnt  opt  proc  root  run  sbin  srv  sys  tmp  usr  var
a0000@mcalab:/ $ cd /dev/
a0000@mcalab:/dev $ ls
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
a0000@mcalab:/dev $ mknod /dev/chrdevbase c 200 0
mknod: /dev/chrdevbase: Permission denied
a0000@mcalab:/dev $ sudo mknod /dev/chrdevbase c 200 0
a0000@mcalab:/dev $ ls
autofs           cuse        gpiochip11  hidraw1  kmsg   loop7         mqueue  ram0   ram4    serial0     tty0   tty18  tty27  tty36  tty45  tty54  tty63      urandom  vcs5   vcsa7        vhci         video26  watchdog
block            disk        gpiochip12  hidraw2  kvm    loop-control  net     ram1   ram5    shm         tty1   tty19  tty28  tty37  tty46  tty55  tty7       usb      vcs6   vcsu         vhost-net    video27  watchdog0
btrfs-control    dma_heap    gpiochip13  hidraw3  log    mapper        null    ram10  ram6    snd         tty10  tty2   tty29  tty38  tty47  tty56  tty8       v4l      vcs7   vcsu1        vhost-vsock  video28  zero
bus              dri         gpiochip4   hidraw4  loop0  media0        pio0    ram11  ram7    spidev0.0   tty11  tty20  tty3   tty39  tty48  tty57  tty9       vcio     vcsa   vcsu2        video19      video29
cec0             fb0         gpiomem0    hwrng    loop1  media1        port    ram12  ram8    spidev0.1   tty12  tty21  tty30  tty4   tty49  tty58  ttyAMA0    vc-mem   vcsa1  vcsu3        video20      video30
cec1             fd          gpiomem1    i2c-1    loop2  media2        ppp     ram13  ram9    spidev10.0  tty13  tty22  tty31  tty40  tty5   tty59  ttyAMA10   vcs      vcsa2  vcsu4        video21      video31
char             full        gpiomem2    i2c-13   loop3  mem           pps0    ram14  random  stderr      tty14  tty23  tty32  tty41  tty50  tty6   ttyprintk  vcs1     vcsa3  vcsu5        video22      video32
chrdevbase       fuse        gpiomem3    i2c-14   loop4  mmcblk0       ptmx    ram15  rfkill  stdin       tty15  tty24  tty33  tty42  tty51  tty60  udmabuf    vcs2     vcsa4  vcsu6        video23      video33
console          gpiochip0   gpiomem4    initctl  loop5  mmcblk0p1     ptp0    ram2   rtc     stdout      tty16  tty25  tty34  tty43  tty52  tty61  uhid       vcs3     vcsa5  vcsu7        video24      video34
cpu_dma_latency  gpiochip10  hidraw0     input    loop6  mmcblk0p2     pts     ram3   rtc0    tty         tty17  tty26  tty35  tty44  tty53  tty62  uinput     vcs4     vcsa6  vga_arbiter  video25      video35
a0000@mcalab:/dev $ cd /
a0000@mcalab:/ $ ls /dev/chrdevbase -l
crw-r--r-- 1 root root 200, 0 Jul 23 23:26 /dev/chrdevbase
a0000@mcalab:/ $ cd /home/a0000/Desktop/linux_driver/class/class_4
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ ./chrdevbaseAPP /dev/chrdevbase 
Can't open file /dev/chrdevbase 
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ cat /proc/devices
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
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ make
make -C /lib/modules/6.12.25+rpt-rpi-2712/build M=/home/a0000/Desktop/linux_driver/class/class_4 modules
make[1]: Entering directory '/home/a0000/kernel-src/linux'
make[1]: Leaving directory '/home/a0000/kernel-src/linux'
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ make clean
make -C /lib/modules/6.12.25+rpt-rpi-2712/build M=/home/a0000/Desktop/linux_driver/class/class_4 clean
make[1]: Entering directory '/home/a0000/kernel-src/linux'
  CLEAN   /home/a0000/Desktop/linux_driver/class/class_4/Module.symvers
make[1]: Leaving directory '/home/a0000/kernel-src/linux'
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ make
make -C /lib/modules/6.12.25+rpt-rpi-2712/build M=/home/a0000/Desktop/linux_driver/class/class_4 modules
make[1]: Entering directory '/home/a0000/kernel-src/linux'
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_4/chrdevbase.o
  MODPOST /home/a0000/Desktop/linux_driver/class/class_4/Module.symvers
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_4/chrdevbase.mod.o
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_4/.module-common.o
  LD [M]  /home/a0000/Desktop/linux_driver/class/class_4/chrdevbase.ko
make[1]: Leaving directory '/home/a0000/kernel-src/linux'
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ sudo cp chrdevbase.ko /lib/modules/$(uname -r)/kernel/drivers/misc/
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ rmmod chrdevbase 
rmmod: ERROR: ../libkmod/libkmod-module.c:856 kmod_module_remove_module() could not remove 'chrdevbase': Operation not permitted
rmmod: ERROR: could not remove module chrdevbase: Operation not permitted
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ sudo rmmod chrdevbase 
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ sudo modprobe chrdevbase
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ lsmod
Module                  Size  Used by
chrdevbase             49152  0
rfcomm                 81920  4
algif_hash             49152  1
algif_skcipher         49152  1
af_alg                 49152  6 algif_hash,algif_skcipher
bnep                   49152  2
binfmt_misc            49152  1
spidev                 49152  0
brcmfmac_wcc           49152  0
hci_uart               65536  0
btbcm                  49152  1 hci_uart
brcmfmac              376832  1 brcmfmac_wcc
brcmutil               49152  1 brcmfmac
bluetooth             638976  31 hci_uart,btbcm,bnep,rfcomm
rpi_hevc_dec           65536  0
cfg80211             1032192  1 brcmfmac
aes_ce_blk             49152  4
pisp_be                49152  0
aes_ce_cipher          49152  1 aes_ce_blk
v4l2_mem2mem           65536  1 rpi_hevc_dec
ghash_ce               49152  0
gf128mul               49152  1 ghash_ce
sha2_ce                49152  0
sha256_arm64           49152  1 sha2_ce
videobuf2_dma_contig    49152  2 pisp_be,rpi_hevc_dec
hid_multitouch         49152  0
videobuf2_memops       49152  1 videobuf2_dma_contig
sha1_ce                49152  0
videobuf2_v4l2         49152  3 pisp_be,rpi_hevc_dec,v4l2_mem2mem
videodev              344064  4 pisp_be,rpi_hevc_dec,videobuf2_v4l2,v4l2_mem2mem
ecdh_generic           49152  2 bluetooth
ecc                    65536  1 ecdh_generic
sha1_generic           49152  1 sha1_ce
rfkill                 49152  6 bluetooth,cfg80211
libaes                 49152  4 aes_ce_cipher,bluetooth,ghash_ce,aes_ce_blk
videobuf2_common       98304  6 pisp_be,rpi_hevc_dec,videobuf2_dma_contig,videobuf2_v4l2,v4l2_mem2mem,videobuf2_memops
raspberrypi_hwmon      49152  0
mc                     81920  6 videodev,pisp_be,rpi_hevc_dec,videobuf2_v4l2,videobuf2_common,v4l2_mem2mem
rp1_pio                65536  0
gpio_keys              49152  0
i2c_brcmstb            49152  0
spi_bcm2835            49152  0
rp1_mailbox            49152  1
spi_dw_mmio            49152  0
rp1_adc                49152  0
pwm_fan                49152  0
rp1_fw                 49152  1 rp1_pio
raspberrypi_gpiomem    49152  0
spi_dw                 49152  1 spi_dw_mmio
i2c_designware_platform    49152  0
nvmem_rmem             49152  0
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
vc4                   425984  16
snd_soc_hdmi_codec     49152  2
snd_soc_core          311296  2 vc4,snd_soc_hdmi_codec
snd_compress           49152  1 snd_soc_core
snd_pcm_dmaengine      49152  1 snd_soc_core
snd_pcm               147456  4 snd_soc_hdmi_codec,snd_compress,snd_soc_core,snd_pcm_dmaengine
snd_timer              65536  1 snd_pcm
snd                   131072  9 snd_soc_hdmi_codec,snd_timer,snd_compress,snd_soc_core,snd_pcm
drm_display_helper     49152  1 vc4
v3d                   212992  8
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
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ dmesg | tail -n 10
[    5.346656] macb 1f00100000.ethernet eth0: configuring for phy/rgmii-id link mode
[    5.349779] macb 1f00100000.ethernet: gem-ptp-timer ptp clock registered.
[    5.360707] brcmfmac: brcmf_cfg80211_set_power_mgmt: power save enabled
[    7.462068] Bluetooth: RFCOMM TTY layer initialized
[    7.462080] Bluetooth: RFCOMM socket layer initialized
[    7.462087] Bluetooth: RFCOMM ver 1.11
[ 6490.564389] chrdevbase: loading out-of-tree module taints kernel.
[ 6490.564837] chrdevbase_init
[ 7137.832881] chrdevbese_exit
[ 7149.822200] chrdevbase_init
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ ./chrdevbaseAPP /dev/chrdevbase 
Can't open file /dev/chrdevbase 
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ sudo rmmod chrdevbase 
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ sudo depmod -a
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ sudo modprobe chrdevbase
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ lsmod
Module                  Size  Used by
chrdevbase             49152  0
rfcomm                 81920  4
algif_hash             49152  1
algif_skcipher         49152  1
af_alg                 49152  6 algif_hash,algif_skcipher
bnep                   49152  2
binfmt_misc            49152  1
spidev                 49152  0
brcmfmac_wcc           49152  0
hci_uart               65536  0
btbcm                  49152  1 hci_uart
brcmfmac              376832  1 brcmfmac_wcc
brcmutil               49152  1 brcmfmac
bluetooth             638976  31 hci_uart,btbcm,bnep,rfcomm
rpi_hevc_dec           65536  0
cfg80211             1032192  1 brcmfmac
aes_ce_blk             49152  4
pisp_be                49152  0
aes_ce_cipher          49152  1 aes_ce_blk
v4l2_mem2mem           65536  1 rpi_hevc_dec
ghash_ce               49152  0
gf128mul               49152  1 ghash_ce
sha2_ce                49152  0
sha256_arm64           49152  1 sha2_ce
videobuf2_dma_contig    49152  2 pisp_be,rpi_hevc_dec
hid_multitouch         49152  0
videobuf2_memops       49152  1 videobuf2_dma_contig
sha1_ce                49152  0
videobuf2_v4l2         49152  3 pisp_be,rpi_hevc_dec,v4l2_mem2mem
videodev              344064  4 pisp_be,rpi_hevc_dec,videobuf2_v4l2,v4l2_mem2mem
ecdh_generic           49152  2 bluetooth
ecc                    65536  1 ecdh_generic
sha1_generic           49152  1 sha1_ce
rfkill                 49152  6 bluetooth,cfg80211
libaes                 49152  4 aes_ce_cipher,bluetooth,ghash_ce,aes_ce_blk
videobuf2_common       98304  6 pisp_be,rpi_hevc_dec,videobuf2_dma_contig,videobuf2_v4l2,v4l2_mem2mem,videobuf2_memops
raspberrypi_hwmon      49152  0
mc                     81920  6 videodev,pisp_be,rpi_hevc_dec,videobuf2_v4l2,videobuf2_common,v4l2_mem2mem
rp1_pio                65536  0
gpio_keys              49152  0
i2c_brcmstb            49152  0
spi_bcm2835            49152  0
rp1_mailbox            49152  1
spi_dw_mmio            49152  0
rp1_adc                49152  0
pwm_fan                49152  0
rp1_fw                 49152  1 rp1_pio
raspberrypi_gpiomem    49152  0
spi_dw                 49152  1 spi_dw_mmio
i2c_designware_platform    49152  0
nvmem_rmem             49152  0
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
vc4                   425984  16
snd_soc_hdmi_codec     49152  2
snd_soc_core          311296  2 vc4,snd_soc_hdmi_codec
snd_compress           49152  1 snd_soc_core
snd_pcm_dmaengine      49152  1 snd_soc_core
snd_pcm               147456  4 snd_soc_hdmi_codec,snd_compress,snd_soc_core,snd_pcm_dmaengine
snd_timer              65536  1 snd_pcm
snd                   131072  9 snd_soc_hdmi_codec,snd_timer,snd_compress,snd_soc_core,snd_pcm
drm_display_helper     49152  1 vc4
v3d                   212992  8
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
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ lsmod
Module                  Size  Used by
chrdevbase             49152  0
rfcomm                 81920  4
algif_hash             49152  1
algif_skcipher         49152  1
af_alg                 49152  6 algif_hash,algif_skcipher
bnep                   49152  2
binfmt_misc            49152  1
spidev                 49152  0
brcmfmac_wcc           49152  0
hci_uart               65536  0
btbcm                  49152  1 hci_uart
brcmfmac              376832  1 brcmfmac_wcc
brcmutil               49152  1 brcmfmac
bluetooth             638976  31 hci_uart,btbcm,bnep,rfcomm
rpi_hevc_dec           65536  0
cfg80211             1032192  1 brcmfmac
aes_ce_blk             49152  4
pisp_be                49152  0
aes_ce_cipher          49152  1 aes_ce_blk
v4l2_mem2mem           65536  1 rpi_hevc_dec
ghash_ce               49152  0
gf128mul               49152  1 ghash_ce
sha2_ce                49152  0
sha256_arm64           49152  1 sha2_ce
videobuf2_dma_contig    49152  2 pisp_be,rpi_hevc_dec
hid_multitouch         49152  0
videobuf2_memops       49152  1 videobuf2_dma_contig
sha1_ce                49152  0
videobuf2_v4l2         49152  3 pisp_be,rpi_hevc_dec,v4l2_mem2mem
videodev              344064  4 pisp_be,rpi_hevc_dec,videobuf2_v4l2,v4l2_mem2mem
ecdh_generic           49152  2 bluetooth
ecc                    65536  1 ecdh_generic
sha1_generic           49152  1 sha1_ce
rfkill                 49152  6 bluetooth,cfg80211
libaes                 49152  4 aes_ce_cipher,bluetooth,ghash_ce,aes_ce_blk
videobuf2_common       98304  6 pisp_be,rpi_hevc_dec,videobuf2_dma_contig,videobuf2_v4l2,v4l2_mem2mem,videobuf2_memops
raspberrypi_hwmon      49152  0
mc                     81920  6 videodev,pisp_be,rpi_hevc_dec,videobuf2_v4l2,videobuf2_common,v4l2_mem2mem
rp1_pio                65536  0
gpio_keys              49152  0
i2c_brcmstb            49152  0
spi_bcm2835            49152  0
rp1_mailbox            49152  1
spi_dw_mmio            49152  0
rp1_adc                49152  0
pwm_fan                49152  0
rp1_fw                 49152  1 rp1_pio
raspberrypi_gpiomem    49152  0
spi_dw                 49152  1 spi_dw_mmio
i2c_designware_platform    49152  0
nvmem_rmem             49152  0
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
vc4                   425984  16
snd_soc_hdmi_codec     49152  2
snd_soc_core          311296  2 vc4,snd_soc_hdmi_codec
snd_compress           49152  1 snd_soc_core
snd_pcm_dmaengine      49152  1 snd_soc_core
snd_pcm               147456  4 snd_soc_hdmi_codec,snd_compress,snd_soc_core,snd_pcm_dmaengine
snd_timer              65536  1 snd_pcm
snd                   131072  9 snd_soc_hdmi_codec,snd_timer,snd_compress,snd_soc_core,snd_pcm
drm_display_helper     49152  1 vc4
v3d                   212992  8
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
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ dmesg | tail -n 10
[    5.360707] brcmfmac: brcmf_cfg80211_set_power_mgmt: power save enabled
[    7.462068] Bluetooth: RFCOMM TTY layer initialized
[    7.462080] Bluetooth: RFCOMM socket layer initialized
[    7.462087] Bluetooth: RFCOMM ver 1.11
[ 6490.564389] chrdevbase: loading out-of-tree module taints kernel.
[ 6490.564837] chrdevbase_init
[ 7137.832881] chrdevbese_exit
[ 7149.822200] chrdevbase_init
[ 7225.024718] chrdevbese_exit
[ 7241.415205] chrdevbase_init
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ cat /proc/devices
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
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ sudo mknod /dev/chrdevbase c 200 0
mknod: /dev/chrdevbase: File exists
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ sudo rm /dev/chrdevbase
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ sudo mknod /dev/chrdevbase c 200 0
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ ls /dev/chrdevbase -l
crw-r--r-- 1 root root 200, 0 Jul 23 23:38 /dev/chrdevbase
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ ./chrdevbaseAPP /dev/chrdevbase 
Can't open file /dev/chrdevbase 
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ sudo ./chrdevbaseAPP /dev/chrdevbase 
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ gcc ./chrdevbaseAPP.c -o chrdevbaseAPP
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ file ./chrdevbaseAPP
./chrdevbaseAPP: ELF 64-bit LSB pie executable, ARM aarch64, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux-aarch64.so.1, BuildID[sha1]=570670209b386dfd58e87ce2eed8a62a341ddbec, for GNU/Linux 3.7.0, not stripped
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ sudo ./chrdevbaseAPP /dev/chrdevbase 
Open file /dev/chrdevbase success
file_descriptor = 3 read file /dev/chrdevbase success
ret = 0 
write file /dev/chrdevbase success
ret = 0 
close file /dev/chrdevbase success
ret = 0 
a0000@mcalab:~/Desktop/linux_driver/class/class_4 $ 
```
## 補充說明
Linux 的 `man`（manual，手冊）系統是如何把內容分門別類的，以及如何指定想看的章節（section）：

1. **man 手冊的章節（section）分類**
   Linux 常見把 `man` 手冊分成以下幾個 section：

   ```
   1  標準指令（Standard commands）
   2  系統呼叫（System calls）
   3  程式庫函式（Library functions）
   4  特殊裝置（Special devices）
   5  檔案格式（File formats）
   6  遊戲和娛樂（Games and toys）
   7  其他雜項（Miscellaneous）
   8  管理員指令（Administrative commands）
   9  Linux 核心例行程式（Linux-specific kernel routines）
   ```

2. **為什麼要指定 section？**
   有些名稱一樣的條目，可能同時在多個 section 出現──例如 `open` 既有一般指令（section 1），也有系統呼叫（section 2）；如果你只下 `man open`，系統預設會先顯示 section 1 的說明，也就是命令介面的 `open`。

   如果你想看 `open(2)`（系統呼叫版）的說明，就要指定章節：

   ```bash
   man 2 open
   ```

3. **怎麼用？**
   在 `man` 後面加數字，就是指定那個 section：

   ```bash
   man 1 ls       # 查看 ls 這個指令（section 1）
   man 3 printf   # 查看 printf 這個 C 函式（section 3）
   man 2 open     # 查看 open 這個系統呼叫（section 2）
   ```

這樣就能精確地跳到你想要的手冊內容了。

```bash
NAME
       open, openat, creat - open and possibly create a file

LIBRARY
       Standard C library (libc, -lc)

SYNOPSIS
       #include <fcntl.h>

       int open(const char *pathname, int flags);
       int open(const char *pathname, int flags, mode_t mode);
```

其中要寫open的話要用`#include <fcntl.h>`