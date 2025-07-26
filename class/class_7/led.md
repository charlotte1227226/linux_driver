# led

## 放到這目錄 
### `sudo cp <your filename>.ko /lib/modules/$(uname -r)/kernel/drivers/misc/`
## 第一次加入要用 
### `sudo depmod -a`
---
### `sudo modprobe <your filename>` 
### 不要加.ko

---
目前都是自己定義主設備號
要記得卸載
```bash
a0000@mcalab:~/Desktop/linux_driver/class/class_7 $ sudo rmmod chrdevbase
```


```bash
a0000@mcalab:~/Desktop/linux_driver/class/class_7 $ make
make -C /lib/modules/6.12.25+rpt-rpi-2712/build M=/home/a0000/Desktop/linux_driver/class/class_7 modules
make[1]: Entering directory '/home/a0000/kernel-src/linux'
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_7/led.o
  MODPOST /home/a0000/Desktop/linux_driver/class/class_7/Module.symvers
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_7/led.mod.o
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_7/.module-common.o
  LD [M]  /home/a0000/Desktop/linux_driver/class/class_7/led.ko
make[1]: Leaving directory '/home/a0000/kernel-src/linux'
a0000@mcalab:~/Desktop/linux_driver/class/class_7 $ sudo cp led.ko /lib/modules/$(uname -r)/kernel/drivers/misc/
a0000@mcalab:~/Desktop/linux_driver/class/class_7 $ ls /lib/modules/$(uname -r)/kernel/drivers/misc/
bcm2835_smi.ko.xz  chrdevbase.ko  eeprom  led.ko  rp1-pio.ko.xz  ti-st  ws2812-pio-rp1.ko.xz
a0000@mcalab:~/Desktop/linux_driver/class/class_7 $ sudo modprobe led
modprobe: FATAL: Module led not found in directory /lib/modules/6.12.34+rpt-rpi-2712
a0000@mcalab:~/Desktop/linux_driver/class/class_7 $ sudo depmod -a
a0000@mcalab:~/Desktop/linux_driver/class/class_7 $ sudo modprobe led
modprobe: ERROR: could not insert 'led': Input/output error
a0000@mcalab:~/Desktop/linux_driver/class/class_7 $ dmesg | tail -n 10
[ 3665.821575] chrdevbase_open
[ 3665.821594] chrdevbase_release
[ 4603.122567] ieee80211 phy0: brcmf_p2p_send_action_frame: Unknown Frame: category 0xa, action 0x8
[ 5821.148938] ieee80211 phy0: brcmf_p2p_send_action_frame: Unknown Frame: category 0xa, action 0x8
[ 6264.803858] ieee80211 phy0: brcmf_p2p_send_action_frame: Unknown Frame: category 0xa, action 0x8
[ 6374.998009] ieee80211 phy0: brcmf_p2p_send_action_frame: Unknown Frame: category 0xa, action 0x8
[ 8039.085056] ieee80211 phy0: brcmf_p2p_send_action_frame: Unknown Frame: category 0xa, action 0x8
[17269.331047] ieee80211 phy0: brcmf_p2p_send_action_frame: Unknown Frame: category 0xa, action 0x8
[18043.166829] ieee80211 phy0: brcmf_p2p_send_action_frame: Unknown Frame: category 0xa, action 0x8
[32758.313332] register chardev failed!
a0000@mcalab:~/Desktop/linux_driver/class/class_7 $ sudo rmmod chrdevbase
a0000@mcalab:~/Desktop/linux_driver/class/class_7 $ sudo modprobe led
a0000@mcalab:~/Desktop/linux_driver/class/class_7 $ dmesg | tail -n 10
[ 4603.122567] ieee80211 phy0: brcmf_p2p_send_action_frame: Unknown Frame: category 0xa, action 0x8
[ 5821.148938] ieee80211 phy0: brcmf_p2p_send_action_frame: Unknown Frame: category 0xa, action 0x8
[ 6264.803858] ieee80211 phy0: brcmf_p2p_send_action_frame: Unknown Frame: category 0xa, action 0x8
[ 6374.998009] ieee80211 phy0: brcmf_p2p_send_action_frame: Unknown Frame: category 0xa, action 0x8
[ 8039.085056] ieee80211 phy0: brcmf_p2p_send_action_frame: Unknown Frame: category 0xa, action 0x8
[17269.331047] ieee80211 phy0: brcmf_p2p_send_action_frame: Unknown Frame: category 0xa, action 0x8
[18043.166829] ieee80211 phy0: brcmf_p2p_send_action_frame: Unknown Frame: category 0xa, action 0x8
[32758.313332] register chardev failed!
[32901.595334] chrdevbese_exit
[32905.357209] led_init
a0000@mcalab:~/Desktop/linux_driver/class/class_7 $ cat /proc/devices
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
200 led
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
a0000@mcalab:~/Desktop/linux_driver/class/class_7 $ lsmod
Module                  Size  Used by
led                    49152  0
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
cec                    65536  1 vc4
drm_shmem_helper       49152  1 v3d
gpu_sched              98304  1 v3d
drm_kms_helper        245760  3 drm_dma_helper,vc4,drm_shmem_helper
drm                   688128  20 gpu_sched,drm_kms_helper,drm_dma_helper,v3d,vc4,drm_shmem_helper,drm_display_helper
drm_panel_orientation_quirks    65536  1 drm
backlight              49152  2 drm_kms_helper,drm
uio_pdrv_genirq        49152  0
uio                    49152  1 uio_pdrv_genirq
a0000@mcalab:~/Desktop/linux_driver/class/class_7 $
```