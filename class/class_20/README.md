# 使用步驟(終端機輸出)
```bash
a0000@mcalab:~/Desktop $ cd ./linux_driver/class/class_20/
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ make clean
make -C /lib/modules/6.12.47+rpt-rpi-2712/build M=/home/a0000/Desktop/linux_driver/class/class_20 clean
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-2712'
  CLEAN   /home/a0000/Desktop/linux_driver/class/class_20/Module.symvers
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-2712'
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ dtc -@ -I dts -O dtb -o dtsled_overlay.dtbo dtsled_overlay.dts
dtsled_overlay.dts:10.22-15.6: Warning (unit_address_vs_reg): /fragment@0/__overlay__/dtsled@0: node has a unit name, but no reg or ranges property
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo dtoverlay -v ./dtsled_overlay.dtbo
DTOVERLAY[debug]: using platform 'bcm2712'
DTOVERLAY[debug]: overlay map loaded
run_cmd: which dtoverlay-pre >/dev/null 2>&1 && dtoverlay-pre
DTOVERLAY[debug]: loading file './dtsled_overlay.dtbo'
DTOVERLAY[debug]: wrote 435 bytes to '/tmp/.dtoverlays/0_dtsled_overlay.dtbo'
DTOVERLAY[debug]: wrote 435 bytes to '/sys/kernel/config/device-tree/overlays/0_dtsled_overlay/dtbo'
run_cmd: which dtoverlay-post >/dev/null 2>&1 && dtoverlay-post
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ ls /sys/kernel/config/device-tree/overlays
0_dtsled_overlay
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ make
make -C /lib/modules/6.12.47+rpt-rpi-2712/build M=/home/a0000/Desktop/linux_driver/class/class_20 modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.47+rpt-rpi-2712'
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_20/dtsled.o
  MODPOST /home/a0000/Desktop/linux_driver/class/class_20/Module.symvers
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_20/dtsled.mod.o
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_20/.module-common.o
  LD [M]  /home/a0000/Desktop/linux_driver/class/class_20/dtsled.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.47+rpt-rpi-2712'
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo insmod ./dtsled.ko
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ dmesg | tail -n 50
[    3.726517] Bluetooth: L2CAP socket layer initialized
[    3.726522] Bluetooth: SCO socket layer initialized
[    3.733810] Bluetooth: HCI UART driver ver 2.3
[    3.733824] Bluetooth: HCI UART protocol H4 registered
[    3.733866] Bluetooth: HCI UART protocol Three-wire (H5) registered
[    3.734056] hci_uart_bcm serial0-0: supply vbat not found, using dummy regulator
[    3.734106] hci_uart_bcm serial0-0: supply vddio not found, using dummy regulator
[    3.735054] brcmfmac: F1 signature read @0x18000000=0x15264345
[    3.736285] brcmfmac: brcmf_fw_alloc_request: using brcm/brcmfmac43455-sdio for chip BCM4345/6
[    3.736447] usbcore: registered new interface driver brcmfmac
[    3.737677] Bluetooth: HCI UART protocol Broadcom registered
[    3.755002] [drm] Initialized vc4 0.0.0 for axi:gpu on minor 1
[    3.805056] input: wch.cn USB2IIC_CTP_CONTROL as /devices/platform/axi/1000120000.pcie/1f00300000.usb/xhci-hcd.1/usb3/3-2/3-2:1.0/0003:1A86:E5E3.0003/input/input19
[    3.805214] hid-multitouch 0003:1A86:E5E3.0003: input,hidraw2: USB HID v1.00 Device [wch.cn USB2IIC_CTP_CONTROL] on usb-xhci-hcd.1-2/input0
[    3.806818] rpi-hevc-dec 1000800000.codec: bcm2712_iommu_of_xlate: MMU 1000005100.iommu
[    3.806825] rpi-hevc-dec 1000800000.codec: bcm2712_iommu_probe_device: MMU 1000005100.iommu
[    3.810003] rpi-hevc-dec 1000800000.codec: bcm2712_iommu_device_group: MMU 1000005100.iommu
[    3.811136] rpi-hevc-dec 1000800000.codec: Adding to iommu group 0
[    3.811143] rpi-hevc-dec 1000800000.codec: bcm2712_iommu_attach_dev: MMU 1000005100.iommu
[    3.819690] rpi-hevc-dec 1000800000.codec: Device registered as /dev/video19
[    3.842353] Console: switching to colour frame buffer device 128x37
[    3.859405] vc4-drm axi:gpu: [drm] fb0: vc4drmfb frame buffer device
[    3.982603] brcmfmac: brcmf_c_process_txcap_blob: no txcap_blob available (err=-2)
[    3.982935] brcmfmac: brcmf_c_preinit_dcmds: Firmware: BCM4345/6 wl0: Aug 29 2023 01:47:08 version 7.45.265 (28bca26 CY) FWID 01-b677b91b
[    4.090083] Bluetooth: hci0: BCM: chip id 107
[    4.090287] Bluetooth: hci0: BCM: features 0x2f
[    4.091337] Bluetooth: hci0: BCM4345C0
[    4.091338] Bluetooth: hci0: BCM4345C0 (003.001.025) build 0000
[    4.102255] Bluetooth: hci0: BCM4345C0 'brcm/BCM4345C0.raspberrypi,5-model-b.hcd' Patch
[    4.700323] Adding 524272k swap on /var/swap.  Priority:-2 extents:4 across:5709824k SS
[    4.790298] Bluetooth: hci0: BCM: features 0x2f
[    4.791662] Bluetooth: hci0: BCM43455 37.4MHz Raspberry Pi 3+-0190
[    4.791664] Bluetooth: hci0: BCM4345C0 (003.001.025) build 0382
[    4.791996] Bluetooth: hci0: BCM: Using default device address (43:45:c0:00:1f:ac)
[    4.823101] Bluetooth: BNEP (Ethernet Emulation) ver 1.3
[    4.823108] Bluetooth: BNEP filters: protocol multicast
[    4.823113] Bluetooth: BNEP socket layer initialized
[    4.824708] Bluetooth: MGMT ver 1.23
[    4.832082] NET: Registered PF_ALG protocol family
[    5.711899] macb 1f00100000.ethernet eth0: PHY [1f00100000.ethernet-ffffffff:01] driver [Broadcom BCM54213PE] (irq=POLL)
[    5.711907] macb 1f00100000.ethernet eth0: configuring for phy/rgmii-id link mode
[    5.715215] macb 1f00100000.ethernet: gem-ptp-timer ptp clock registered.
[    5.725504] brcmfmac: brcmf_cfg80211_set_power_mgmt: power save enabled
[    7.852705] Bluetooth: RFCOMM TTY layer initialized
[    7.852717] Bluetooth: RFCOMM socket layer initialized
[    7.852724] Bluetooth: RFCOMM ver 1.11
[ 4613.476585] logitech-hidpp-device 0003:046D:4091.0005: HID++ 4.5 device connected.
[ 5887.352662] dtsled: loading out-of-tree module taints kernel.
[ 5887.353633] dtsled dtsled@0: probed: /dev/myled ready
[ 5887.353781] dtsled: loaded
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ ls -l /dev/myled
crw------- 1 root root 509, 0 Dec 26 20:17 /dev/myled
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ echo 1 | sudo tee /dev/myled
1
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ echo 0 | sudo tee /dev/myled
0
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ gcc -O2 -Wall -o ledAPP ledAPP.c
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo ./ledAPP /dev/myled on
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo ./ledAPP /dev/myled off
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo ./ledAPP /dev/myled toggle
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo ./ledAPP /dev/myled blink 500 500

a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo ./ledAPP /dev/myled blink_stop
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo rmmod dtsled
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ dmesg | tail -n 50
[    3.726522] Bluetooth: SCO socket layer initialized
[    3.733810] Bluetooth: HCI UART driver ver 2.3
[    3.733824] Bluetooth: HCI UART protocol H4 registered
[    3.733866] Bluetooth: HCI UART protocol Three-wire (H5) registered
[    3.734056] hci_uart_bcm serial0-0: supply vbat not found, using dummy regulator
[    3.734106] hci_uart_bcm serial0-0: supply vddio not found, using dummy regulator
[    3.735054] brcmfmac: F1 signature read @0x18000000=0x15264345
[    3.736285] brcmfmac: brcmf_fw_alloc_request: using brcm/brcmfmac43455-sdio for chip BCM4345/6
[    3.736447] usbcore: registered new interface driver brcmfmac
[    3.737677] Bluetooth: HCI UART protocol Broadcom registered
[    3.755002] [drm] Initialized vc4 0.0.0 for axi:gpu on minor 1
[    3.805056] input: wch.cn USB2IIC_CTP_CONTROL as /devices/platform/axi/1000120000.pcie/1f00300000.usb/xhci-hcd.1/usb3/3-2/3-2:1.0/0003:1A86:E5E3.0003/input/input19
[    3.805214] hid-multitouch 0003:1A86:E5E3.0003: input,hidraw2: USB HID v1.00 Device [wch.cn USB2IIC_CTP_CONTROL] on usb-xhci-hcd.1-2/input0
[    3.806818] rpi-hevc-dec 1000800000.codec: bcm2712_iommu_of_xlate: MMU 1000005100.iommu
[    3.806825] rpi-hevc-dec 1000800000.codec: bcm2712_iommu_probe_device: MMU 1000005100.iommu
[    3.810003] rpi-hevc-dec 1000800000.codec: bcm2712_iommu_device_group: MMU 1000005100.iommu
[    3.811136] rpi-hevc-dec 1000800000.codec: Adding to iommu group 0
[    3.811143] rpi-hevc-dec 1000800000.codec: bcm2712_iommu_attach_dev: MMU 1000005100.iommu
[    3.819690] rpi-hevc-dec 1000800000.codec: Device registered as /dev/video19
[    3.842353] Console: switching to colour frame buffer device 128x37
[    3.859405] vc4-drm axi:gpu: [drm] fb0: vc4drmfb frame buffer device
[    3.982603] brcmfmac: brcmf_c_process_txcap_blob: no txcap_blob available (err=-2)
[    3.982935] brcmfmac: brcmf_c_preinit_dcmds: Firmware: BCM4345/6 wl0: Aug 29 2023 01:47:08 version 7.45.265 (28bca26 CY) FWID 01-b677b91b
[    4.090083] Bluetooth: hci0: BCM: chip id 107
[    4.090287] Bluetooth: hci0: BCM: features 0x2f
[    4.091337] Bluetooth: hci0: BCM4345C0
[    4.091338] Bluetooth: hci0: BCM4345C0 (003.001.025) build 0000
[    4.102255] Bluetooth: hci0: BCM4345C0 'brcm/BCM4345C0.raspberrypi,5-model-b.hcd' Patch
[    4.700323] Adding 524272k swap on /var/swap.  Priority:-2 extents:4 across:5709824k SS
[    4.790298] Bluetooth: hci0: BCM: features 0x2f
[    4.791662] Bluetooth: hci0: BCM43455 37.4MHz Raspberry Pi 3+-0190
[    4.791664] Bluetooth: hci0: BCM4345C0 (003.001.025) build 0382
[    4.791996] Bluetooth: hci0: BCM: Using default device address (43:45:c0:00:1f:ac)
[    4.823101] Bluetooth: BNEP (Ethernet Emulation) ver 1.3
[    4.823108] Bluetooth: BNEP filters: protocol multicast
[    4.823113] Bluetooth: BNEP socket layer initialized
[    4.824708] Bluetooth: MGMT ver 1.23
[    4.832082] NET: Registered PF_ALG protocol family
[    5.711899] macb 1f00100000.ethernet eth0: PHY [1f00100000.ethernet-ffffffff:01] driver [Broadcom BCM54213PE] (irq=POLL)
[    5.711907] macb 1f00100000.ethernet eth0: configuring for phy/rgmii-id link mode
[    5.715215] macb 1f00100000.ethernet: gem-ptp-timer ptp clock registered.
[    5.725504] brcmfmac: brcmf_cfg80211_set_power_mgmt: power save enabled
[    7.852705] Bluetooth: RFCOMM TTY layer initialized
[    7.852717] Bluetooth: RFCOMM socket layer initialized
[    7.852724] Bluetooth: RFCOMM ver 1.11
[ 4613.476585] logitech-hidpp-device 0003:046D:4091.0005: HID++ 4.5 device connected.
[ 5887.352662] dtsled: loading out-of-tree module taints kernel.
[ 5887.353633] dtsled dtsled@0: probed: /dev/myled ready
[ 5887.353781] dtsled: loaded
[ 6229.606962] dtsled: unloaded
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ ls -l /dev/myled
ls: cannot access '/dev/myled': No such file or directory
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ 
```