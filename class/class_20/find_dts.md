# 「從零到會亮＋可開機自動」

* Driver：`dtsled.c`（已修掉 deadlock／remove 回傳型別）
* Overlay：`dtsled_overlay.dts`
* App：`ledAPP.c`
* Makefile：編 kernel module 用

---

# A. 建好 overlay（DTBO）

1. 產生 `.dtbo`

```bash
dtc -@ -I dts -O dtb -o dtsled_overlay.dtbo dtsled_overlay.dts
```

（dtc 的小黃字警告可忽略）

2. 先「臨時套用」到目前跑著的系統

```bash
sudo dtoverlay -v ./dtsled_overlay.dtbo
# 驗證是否掛上
ls /sys/kernel/config/device-tree/overlays
```

---

# B. 編譯並載入驅動（.ko）

1. 編譯

```bash
make
```

2. 載入（暫時）

```bash
sudo insmod ./dtsled.ko
dmesg | tail -n 50
# 看到：
# dtsled: loaded
# dtsled dtsled@0: probed: /dev/myled ready
```

---

# C. 測試（直接對 /dev 或用 ledAPP）

### 直接對 /dev 測

```bash
ls -l /dev/myled
echo 1 | sudo tee /dev/myled        # 亮
echo 0 | sudo tee /dev/myled        # 滅
echo t | sudo tee /dev/myled        # 切換
sudo cat /dev/myled                 # 讀 0/1
echo "blink 200 800" | sudo tee /dev/myled
echo "blink_stop" | sudo tee /dev/myled
```

### 用 ledAPP（你已成功編過，重貼一次）

```bash
gcc -O2 -Wall -o ledAPP ledAPP.c
sudo ./ledAPP /dev/myled on
sudo ./ledAPP /dev/myled off
sudo ./ledAPP /dev/myled toggle
sudo ./ledAPP /dev/myled read
sudo ./ledAPP /dev/myled blink 500 500
sudo ./ledAPP /dev/myled blink_stop
```

> 想不必每次都 sudo：
> 臨時 `sudo chmod 666 /dev/myled`；
> 永久可加 udev 規則：
>
> ```bash
> sudo bash -c 'cat >/etc/udev/rules.d/99-myled.rules << "EOF"
> KERNEL=="myled", MODE="0666", TAG+="uaccess"
> EOF'
> sudo udevadm control --reload
> sudo udevadm trigger
> ```

---

# D. 開機自動生效（可選，但通常會要）

1. 讓 overlay 開機就套上
   把檔案放到韌體 overlays 目錄，並寫入 `config.txt`：

```bash
sudo cp dtsled_overlay.dtbo /boot/firmware/overlays/
echo dtoverlay=dtsled_overlay | sudo tee -a /boot/firmware/config.txt
```

2. 安裝驅動到系統模組目錄，並建立索引

```bash
sudo install -D -m 644 dtsled.ko /lib/modules/$(uname -r)/kernel/drivers/misc/dtsled.ko
sudo depmod -a
```

> 正常情況下，開機時 overlay 會創建 platform 裝置並觸發 `modprobe` 自動載入你的模組。
> 若想保險，可再加一行讓系統開機強制載入：
>
> ```bash
> echo dtsled | sudo tee /etc/modules-load.d/dtsled.conf
> ```

---

# E. 卸載／重載（debug 用）

1. 停止使用者程式，確保裝置沒有被開啟：

```bash
sudo fuser -v /dev/myled           # 看誰佔用
# 若有 PID：sudo kill <PID>
```

2. 卸載模組

```bash
sudo rmmod dtsled
```

3. 移除臨時套用的 overlay（當前開機那個）

```bash
ls /sys/kernel/config/device-tree/overlays
# 例如看到 0_dtsled_overlay
sudo dtoverlay -r 0_dtsled_overlay    # 或直接 rmdir 那個資料夾
```

（下次要重來：先 dtoverlay 套上，再 insmod）

---

# F. 常見檢查點

* `echo 1 > /dev/myled` 卡住
  → 你已換成最新版 `dtsled.c`（避免在持鎖下 `cancel_delayed_work_sync`），一般就不會卡。若還卡，多半是有程式卡著 `/dev/myled`：`sudo fuser -v /dev/myled` 找出並 kill。

* 沒看到 `/dev/myled`
  → 看 dmesg 是否有 `probed: /dev/myled ready`；若沒有，通常是 overlay 沒套成功或 pin 定義錯。

* LED 沒亮
  → 先 `echo 1`；確定 GPIO 腳位/接線（GPIO26→電阻→LED→GND，ACTIVE\_HIGH 設計）、地線共地。

---

照這份流程走，你已經可以：臨時測試 → 驗證 → 讓它開機自動生效。
要我幫你做一個最小的 `repo` 結構（含檔案樹與 Makefile/指令）也可以，我直接幫你排好目錄。


# 尋找設備樹
```bash
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo apt install -y device-tree-compiler
Reading package lists... Done
Building dependency tree... Done
Reading state information... Done
device-tree-compiler is already the newest version (1.6.1-4+b1).
The following packages were automatically installed and are no longer required:
  libbasicusageenvironment1 libgroupsock8 liblivemedia77 python3-v4l2
Use 'sudo apt autoremove' to remove them.
0 upgraded, 0 newly installed, 0 to remove and 0 not upgraded.
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo dtc -I fs -O dts /sys/firmware/devicetree/base > /tmp/running.dts
<stdout>: Warning (reg_format): /axi/vc_mem:reg: property has invalid length (12 bytes) (#address-cells == 2, #size-cells == 2)
<stdout>: Warning (ranges_format): /chosen:ranges: empty "ranges" property but its #address-cells (1) differs from / (2)
<stdout>: Warning (unit_address_vs_reg): /soc@107c000000/nvmem/nvmem_otp: node has a reg or ranges property, but no unit name
<stdout>: Warning (unit_address_vs_reg): /soc@107c000000/nvmem/nvmem_mac: node has a reg or ranges property, but no unit name
<stdout>: Warning (unit_address_vs_reg): /soc@107c000000/nvmem/nvmem_priv: node has a reg or ranges property, but no unit name
<stdout>: Warning (unit_address_vs_reg): /soc@107c000000/nvmem/nvmem_cust: node has a reg or ranges property, but no unit name
<stdout>: Warning (unit_address_vs_reg): /axi: node has a reg or ranges property, but no unit name
<stdout>: Warning (unit_address_vs_reg): /axi/pcie@1000120000/rp1: node has a reg or ranges property, but no unit name
<stdout>: Warning (unit_address_vs_reg): /axi/vc_mem: node has a reg or ranges property, but no unit name
<stdout>: Warning (pci_device_reg): Failed prerequisite 'reg_format'
<stdout>: Warning (pci_device_bus_num): Failed prerequisite 'reg_format'
<stdout>: Warning (simple_bus_reg): Failed prerequisite 'reg_format'
<stdout>: Warning (i2c_bus_reg): Failed prerequisite 'reg_format'
<stdout>: Warning (spi_bus_reg): Failed prerequisite 'reg_format'
<stdout>: Warning (avoid_unnecessary_addr_size): /soc@107c000000/firmware: unnecessary #address-cells/#size-cells without "ranges" or child "reg" property
<stdout>: Warning (avoid_unnecessary_addr_size): /clocks: unnecessary #address-cells/#size-cells without "ranges" or child "reg" property
<stdout>: Warning (avoid_unnecessary_addr_size): /axi/pcie@1000120000/rp1/spi@60000: unnecessary #address-cells/#size-cells without "ranges" or child "reg" property
<stdout>: Warning (avoid_unnecessary_addr_size): /axi/pcie@1000120000/rp1/spi@6c000: unnecessary #address-cells/#size-cells without "ranges" or child "reg" property
<stdout>: Warning (unique_unit_address): /soc@107c000000/gpiomem@7d510700: duplicate unit-address (also used in node /soc@107c000000/pinctrl@7d510700)
<stdout>: Warning (unique_unit_address): /soc@107c000000/gpiomem@7d504100: duplicate unit-address (also used in node /soc@107c000000/pinctrl@7d504100)
<stdout>: Warning (unique_unit_address): /soc@107c000000/gpio@7d508500: duplicate unit-address (also used in node /soc@107c000000/gpiomem@7d508500)
<stdout>: Warning (unique_unit_address): /soc@107c000000/gpio@7d517c00: duplicate unit-address (also used in node /soc@107c000000/gpiomem@7d517c00)
<stdout>: Warning (unique_unit_address): /reserved-memory/atf@0: duplicate unit-address (also used in node /reserved-memory/nvram@0)
<stdout>: Warning (unique_unit_address): /axi/pcie@1000120000/rp1/csi@128000: duplicate unit-address (also used in node /axi/pcie@1000120000/rp1/dsi@128000)
<stdout>: Warning (unique_unit_address): /axi/pcie@1000120000/rp1/dsi@110000: duplicate unit-address (also used in node /axi/pcie@1000120000/rp1/csi@110000)
<stdout>: Warning (unique_unit_address): /axi/pcie@1000120000/rp1/gpiomem@d0000: duplicate unit-address (also used in node /axi/pcie@1000120000/rp1/gpio@d0000)
<stdout>: Warning (clocks_property): /soc@107c000000/hdmi@7ef05700:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /soc@107c000000/hdmi@7ef05700:clocks: cell 2 is not a phandle reference
<stdout>: Warning (clocks_property): /soc@107c000000/hdmi@7ef05700:clocks: cell 4 is not a phandle reference
<stdout>: Warning (clocks_property): /soc@107c000000/hdmi@7ef05700:clocks: cell 6 is not a phandle reference
<stdout>: Warning (clocks_property): /soc@107c000000/clock@7c700000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /soc@107c000000/mmc@fff000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /soc@107c000000/hdmi@7ef00700:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /soc@107c000000/hdmi@7ef00700:clocks: cell 2 is not a phandle reference
<stdout>: Warning (clocks_property): /soc@107c000000/hdmi@7ef00700:clocks: cell 4 is not a phandle reference
<stdout>: Warning (clocks_property): /soc@107c000000/hdmi@7ef00700:clocks: cell 6 is not a phandle reference
<stdout>: Warning (clocks_property): /soc@107c000000/serial@7d001000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /soc@107c000000/serial@7d001000:clocks: cell 1 is not a phandle reference
<stdout>: Warning (clocks_property): /soc@107c000000/i2c@7d005600:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /soc@107c000000/spi@7d004000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /clocks/clksrc_gp0:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /clocks/clksrc_gp5:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /clocks/clksrc_gp3:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /clocks/clksrc_gp1:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /clocks/clksrc_gp4:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /clocks/clksrc_gp2:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /hvs@107c580000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /hvs@107c580000:clocks: cell 2 is not a phandle reference
<stdout>: Warning (clocks_property): /__symbols__:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /__symbols__:clocks: Could not get phandle node for (cell 0)
<stdout>: Warning (clocks_property): /axi/mmc@1100000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/dsi@110000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/dsi@110000:clocks: cell 2 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/dsi@110000:clocks: cell 4 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/dsi@110000:clocks: cell 6 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/dsi@110000:clocks: cell 7 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/serial@34000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/serial@34000:clocks: cell 2 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/spi@60000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/spi@58000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/i2c@7c000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/mmc@184000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/mmc@184000:clocks: cell 2 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/mmc@184000:clocks: cell 3 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/mmc@184000:clocks: cell 5 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/i2c@84000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/i2s@a4000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/serial@3c000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/serial@3c000:clocks: cell 2 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/i2c@70000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/csi@128000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/dpi@148000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/dpi@148000:clocks: cell 2 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/dpi@148000:clocks: cell 4 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/serial@44000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/serial@44000:clocks: cell 2 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/spi@68000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/spi@4c000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/clocks@18000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/serial@30000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/serial@30000:clocks: cell 2 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/vec@144000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/spi@54000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/pwm@9c000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/dsi@128000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/dsi@128000:clocks: cell 2 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/dsi@128000:clocks: cell 4 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/dsi@128000:clocks: cell 6 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/dsi@128000:clocks: cell 7 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/mmc@180000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/mmc@180000:clocks: cell 2 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/mmc@180000:clocks: cell 3 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/mmc@180000:clocks: cell 5 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/adc@c8000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/i2c@80000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/i2c@78000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/i2s@a0000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/audio_out@94000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/spi@5c000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/ethernet@100000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/ethernet@100000:clocks: cell 2 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/ethernet@100000:clocks: cell 4 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/ethernet@100000:clocks: cell 6 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/serial@40000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/serial@40000:clocks: cell 2 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/serial@38000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/serial@38000:clocks: cell 2 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/dma@188000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/dma@188000:clocks: cell 2 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/spi@64000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/csi@110000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/sdio_clk1@b4004:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/sdio_clk1@b4004:clocks: cell 1 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/spi@50000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/i2c@88000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/sdio_clk0@b0004:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/sdio_clk0@b0004:clocks: cell 1 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/i2s@a8000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/pwm@98000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/spi@6c000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pcie@1000120000/rp1/i2c@74000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/v3d@2000000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/codec@800000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/usb@480000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (clocks_property): /axi/pisp_be@880000:clocks: cell 0 is not a phandle reference
<stdout>: Warning (cooling_device_property): /thermal-zones/cpu-thermal/cooling-maps/tepid:cooling-device: cell 0 is not a phandle reference
<stdout>: Warning (cooling_device_property): /thermal-zones/cpu-thermal/cooling-maps/melt:cooling-device: cell 0 is not a phandle reference
<stdout>: Warning (cooling_device_property): /thermal-zones/cpu-thermal/cooling-maps/warm:cooling-device: cell 0 is not a phandle reference
<stdout>: Warning (cooling_device_property): /thermal-zones/cpu-thermal/cooling-maps/vhot:cooling-device: cell 0 is not a phandle reference
<stdout>: Warning (cooling_device_property): /thermal-zones/cpu-thermal/cooling-maps/hot:cooling-device: cell 0 is not a phandle reference
<stdout>: Warning (dmas_property): /soc@107c000000/hdmi@7ef05700:dmas: cell 0 is not a phandle reference
<stdout>: Warning (dmas_property): /soc@107c000000/hdmi@7ef00700:dmas: cell 0 is not a phandle reference
<stdout>: Warning (dmas_property): /soc@107c000000/spi@7d004000:dmas: cell 0 is not a phandle reference
<stdout>: Warning (dmas_property): /soc@107c000000/spi@7d004000:dmas: cell 2 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@60000:dmas: cell 0 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@60000:dmas: cell 2 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@58000:dmas: cell 0 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@58000:dmas: cell 2 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/i2s@a4000:dmas: cell 0 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/i2s@a4000:dmas: cell 2 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@68000:dmas: cell 0 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@68000:dmas: cell 2 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@4c000:dmas: cell 0 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@4c000:dmas: cell 2 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/pio@178000:dmas: cell 0 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/pio@178000:dmas: cell 2 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/pio@178000:dmas: cell 4 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/pio@178000:dmas: cell 6 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/pio@178000:dmas: cell 8 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/pio@178000:dmas: cell 10 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/pio@178000:dmas: cell 12 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/pio@178000:dmas: cell 14 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@54000:dmas: cell 0 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@54000:dmas: cell 2 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/i2s@a0000:dmas: cell 0 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/i2s@a0000:dmas: cell 2 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/audio_out@94000:dmas: cell 0 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@5c000:dmas: cell 0 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@5c000:dmas: cell 2 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@64000:dmas: cell 0 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@64000:dmas: cell 2 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@50000:dmas: cell 0 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@50000:dmas: cell 2 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@6c000:dmas: cell 0 is not a phandle reference
<stdout>: Warning (dmas_property): /axi/pcie@1000120000/rp1/spi@6c000:dmas: cell 2 is not a phandle reference
<stdout>: Warning (iommus_property): /hvs@107c580000:iommus: cell 0 is not a phandle reference
<stdout>: Warning (iommus_property): /axi/gpu:iommus: cell 0 is not a phandle reference
<stdout>: Warning (iommus_property): /axi/pcie@1000120000/rp1/dsi@110000:iommus: cell 0 is not a phandle reference
<stdout>: Warning (iommus_property): /axi/pcie@1000120000/rp1/csi@128000:iommus: cell 0 is not a phandle reference
<stdout>: Warning (iommus_property): /axi/pcie@1000120000/rp1/dpi@148000:iommus: cell 0 is not a phandle reference
<stdout>: Warning (iommus_property): /axi/pcie@1000120000/rp1/vec@144000:iommus: cell 0 is not a phandle reference
<stdout>: Warning (iommus_property): /axi/pcie@1000120000/rp1/dsi@128000:iommus: cell 0 is not a phandle reference
<stdout>: Warning (iommus_property): /axi/pcie@1000120000/rp1/csi@110000:iommus: cell 0 is not a phandle reference
<stdout>: Warning (iommus_property): /axi/codec@800000:iommus: cell 0 is not a phandle reference
<stdout>: Warning (iommus_property): /axi/pisp_be@880000:iommus: cell 0 is not a phandle reference
<stdout>: Warning (mboxes_property): /soc@107c000000/firmware:mboxes: cell 0 is not a phandle reference
<stdout>: Warning (mboxes_property): /rp1_firmware:mboxes: cell 0 is not a phandle reference
<stdout>: Warning (msi_parent_property): /axi/pcie@1000110000:msi-parent: cell 0 is not a phandle reference
<stdout>: Warning (msi_parent_property): /axi/pcie@1000120000:msi-parent: cell 0 is not a phandle reference
<stdout>: Warning (msi_parent_property): /axi/pcie@1000100000:msi-parent: cell 0 is not a phandle reference
<stdout>: Warning (phys_property): /axi/usb@480000:phys: cell 0 is not a phandle reference
<stdout>: Warning (power_domains_property): /axi/v3d@2000000:power-domains: cell 0 is not a phandle reference
<stdout>: Warning (power_domains_property): /axi/usb@480000:power-domains: cell 0 is not a phandle reference
<stdout>: Warning (pwms_property): /cooling_fan:pwms: cell 0 is not a phandle reference
<stdout>: Warning (resets_property): /soc@107c000000/hdmi@7ef05700:resets: cell 0 is not a phandle reference
<stdout>: Warning (resets_property): /soc@107c000000/hdmi@7ef00700:resets: cell 0 is not a phandle reference
<stdout>: Warning (resets_property): /axi/pcie@1000110000:resets: cell 0 is not a phandle reference
<stdout>: Warning (resets_property): /axi/pcie@1000110000:resets: cell 1 is not a phandle reference
<stdout>: Warning (resets_property): /axi/pcie@1000120000:resets: cell 0 is not a phandle reference
<stdout>: Warning (resets_property): /axi/pcie@1000120000:resets: cell 1 is not a phandle reference
<stdout>: Warning (resets_property): /axi/v3d@2000000:resets: cell 0 is not a phandle reference
<stdout>: Warning (resets_property): /axi/pcie@1000100000:resets: cell 0 is not a phandle reference
<stdout>: Warning (resets_property): /axi/pcie@1000100000:resets: cell 1 is not a phandle reference
<stdout>: Warning (thermal_sensors_property): /thermal-zones/cpu-thermal:thermal-sensors: cell 0 is not a phandle reference
<stdout>: Warning (gpios_property): /wl-on-reg:gpio: cell 0 is not a phandle reference
<stdout>: Warning (gpios_property): /soc@107c000000/serial@7d50c000/bluetooth:shutdown-gpios: cell 0 is not a phandle reference
<stdout>: Warning (gpios_property): /soc@107c000000/mmc@fff000:cd-gpios: cell 0 is not a phandle reference
<stdout>: Warning (gpios_property): /soc@107c000000/spi@7d004000:cs-gpios: cell 0 is not a phandle reference
<stdout>: Warning (gpios_property): /cam0_reg:gpio: cell 0 is not a phandle reference
<stdout>: Warning (gpios_property): /leds/led-act:gpios: cell 0 is not a phandle reference
<stdout>: Warning (gpios_property): /leds/led-pwr:gpios: cell 0 is not a phandle reference
<stdout>: Warning (gpios_property): /cam1_reg:gpio: cell 0 is not a phandle reference
<stdout>: Warning (gpios_property): /sd-io-1v8-reg:gpios: cell 0 is not a phandle reference
<stdout>: Warning (gpios_property): /__symbols__:gpio: cell 0 is not a phandle reference
<stdout>: Warning (gpios_property): /__symbols__:gpio: Could not get phandle node for (cell 0)
<stdout>: Warning (gpios_property): /axi/pcie@1000120000/rp1/ethernet@100000:phy-reset-gpios: cell 0 is not a phandle reference
<stdout>: Warning (gpios_property): /axi/pcie@1000120000/rp1/spi@50000:cs-gpios: cell 0 is not a phandle reference
<stdout>: Warning (gpios_property): /axi/pcie@1000120000/rp1/spi@50000:cs-gpios: cell 3 is not a phandle reference
<stdout>: Warning (gpios_property): /pwr_button/pwr:gpios: cell 0 is not a phandle reference
<stdout>: Warning (gpios_property): /sd-vcc-reg:gpios: cell 0 is not a phandle reference
<stdout>: Warning (interrupt_provider): /soc@107c000000/interrupt-controller@7c502000: Missing #address-cells in interrupt provider
<stdout>: Warning (interrupt_provider): /soc@107c000000/gpio@7d508500: Missing #address-cells in interrupt provider
<stdout>: Warning (interrupt_provider): /soc@107c000000/interrupt-controller@7fff9000: Missing #address-cells in interrupt provider
<stdout>: Warning (interrupt_provider): /soc@107c000000/interrupt-controller@7d510600: Missing #address-cells in interrupt provider
<stdout>: Warning (interrupt_provider): /soc@107c000000/intc@7d503000: Missing #address-cells in interrupt provider
<stdout>: Warning (interrupt_provider): /soc@107c000000/intc@7d508400: Missing #address-cells in interrupt provider
<stdout>: Warning (interrupt_provider): /soc@107c000000/intc@7d508380: Missing #address-cells in interrupt provider
<stdout>: Warning (interrupt_provider): /soc@107c000000/intc@7d517ac0: Missing #address-cells in interrupt provider
<stdout>: Warning (interrupt_provider): /soc@107c000000/interrupt-controller@7d517000: Missing #address-cells in interrupt provider
<stdout>: Warning (interrupt_provider): /axi/pcie@1000120000/rp1/gpio@d0000: Missing #address-cells in interrupt provider
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ head -n 20 /tmp/running.dts
/dts-v1/;

/ {
        #address-cells = <0x02>;
        memreserve = <0x3f800000 0x800000>;
        model = "Raspberry Pi 5 Model B Rev 1.0";
        serial-number = "f5a81ba4567521a0";
        #size-cells = <0x01>;
        interrupt-parent = <0x01>;
        compatible = "raspberrypi,5-model-b\0brcm,bcm2712";

        cooling_fan {
                rpm-regmap = <0x61>;
                cooling-levels = <0x00 0x4b 0x7d 0xaf 0xfa>;
                cooling-max-state = <0x03>;
                compatible = "pwm-fan";
                status = "okay";
                rpm-offset = <0x3c>;
                phandle = <0x54>;
                pwms = <0x61 0x03 0xa25e 0x01>;
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ 
```

# 終端機結果
```bash
a0000@mcalab:~/Desktop $ cd ./linux_driver/class/class_20
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
make -C /lib/modules/6.12.34+rpt-rpi-2712/build M=/home/a0000/Desktop/linux_driver/class/class_20 modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.34+rpt-rpi-2712'
make[1]: Leaving directory '/usr/src/linux-headers-6.12.34+rpt-rpi-2712'
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ make clean
make -C /lib/modules/6.12.34+rpt-rpi-2712/build M=/home/a0000/Desktop/linux_driver/class/class_20 clean
make[1]: Entering directory '/usr/src/linux-headers-6.12.34+rpt-rpi-2712'
  CLEAN   /home/a0000/Desktop/linux_driver/class/class_20/Module.symvers
make[1]: Leaving directory '/usr/src/linux-headers-6.12.34+rpt-rpi-2712'
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ make
make -C /lib/modules/6.12.34+rpt-rpi-2712/build M=/home/a0000/Desktop/linux_driver/class/class_20 modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.34+rpt-rpi-2712'
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_20/dtsled.o
  MODPOST /home/a0000/Desktop/linux_driver/class/class_20/Module.symvers
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_20/dtsled.mod.o
  CC [M]  /home/a0000/Desktop/linux_driver/class/class_20/.module-common.o
  LD [M]  /home/a0000/Desktop/linux_driver/class/class_20/dtsled.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.34+rpt-rpi-2712'
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo insmod ./dtsled.ko
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ dmesg | tail -n 50
[    3.383341] Bluetooth: HCI device and connection manager initialized
[    3.383348] Bluetooth: HCI socket layer initialized
[    3.383351] Bluetooth: L2CAP socket layer initialized
[    3.383356] Bluetooth: SCO socket layer initialized
[    3.390632] cfg80211: Loading compiled-in X.509 certificates for regulatory database
[    3.395646] Loaded X.509 cert 'benh@debian.org: 577e021cb980e0e820821ba7b54b4961b8b4fadf'
[    3.395847] Loaded X.509 cert 'romain.perier@gmail.com: 3abbc6ec146e09d1b6016ab9d6cf71dd233f0328'
[    3.396038] Loaded X.509 cert 'sforshee: 00b28ddf47aef9cea7'
[    3.396229] Loaded X.509 cert 'wens: 61c038651aabdcf94bd0ac7ff06c7248db18c600'
[    3.398698] Bluetooth: HCI UART driver ver 2.3
[    3.398703] Bluetooth: HCI UART protocol H4 registered
[    3.398724] Bluetooth: HCI UART protocol Three-wire (H5) registered
[    3.398829] hci_uart_bcm serial0-0: supply vbat not found, using dummy regulator
[    3.398882] hci_uart_bcm serial0-0: supply vddio not found, using dummy regulator
[    3.398905] Bluetooth: HCI UART protocol Broadcom registered
[    3.402330] Console: switching to colour frame buffer device 128x37
[    3.419385] vc4-drm axi:gpu: [drm] fb0: vc4drmfb frame buffer device
[    3.425923] rpi-hevc-dec 1000800000.codec: Device registered as /dev/video19
[    3.444852] input: wch.cn USB2IIC_CTP_CONTROL as /devices/platform/axi/1000120000.pcie/1f00300000.usb/xhci-hcd.1/usb3/3-2/3-2:1.0/0003:1A86:E5E3.0001/input/input19
[    3.447416] hid-multitouch 0003:1A86:E5E3.0001: input,hidraw0: USB HID v1.00 Device [wch.cn USB2IIC_CTP_CONTROL] on usb-xhci-hcd.1-2/input0
[    3.447911] brcmfmac: F1 signature read @0x18000000=0x15264345
[    3.450913] brcmfmac: brcmf_fw_alloc_request: using brcm/brcmfmac43455-sdio for chip BCM4345/6
[    3.453374] usbcore: registered new interface driver brcmfmac
[    3.672355] brcmfmac: brcmf_c_process_txcap_blob: no txcap_blob available (err=-2)
[    3.672588] brcmfmac: brcmf_c_preinit_dcmds: Firmware: BCM4345/6 wl0: Aug 29 2023 01:47:08 version 7.45.265 (28bca26 CY) FWID 01-b677b91b
[    3.750042] Bluetooth: hci0: BCM: chip id 107
[    3.750260] Bluetooth: hci0: BCM: features 0x2f
[    3.751308] Bluetooth: hci0: BCM4345C0
[    3.751311] Bluetooth: hci0: BCM4345C0 (003.001.025) build 0000
[    3.753844] Bluetooth: hci0: BCM4345C0 'brcm/BCM4345C0.raspberrypi,5-model-b.hcd' Patch
[    4.400475] Adding 524272k swap on /var/swap.  Priority:-2 extents:4 across:5709824k SS
[    4.426312] Bluetooth: hci0: BCM: features 0x2f
[    4.427708] Bluetooth: hci0: BCM43455 37.4MHz Raspberry Pi 3+-0190
[    4.427714] Bluetooth: hci0: BCM4345C0 (003.001.025) build 0382
[    4.428047] Bluetooth: hci0: BCM: Using default device address (43:45:c0:00:1f:ac)
[    4.478670] Bluetooth: BNEP (Ethernet Emulation) ver 1.3
[    4.478677] Bluetooth: BNEP filters: protocol multicast
[    4.478682] Bluetooth: BNEP socket layer initialized
[    4.480719] Bluetooth: MGMT ver 1.23
[    4.486675] NET: Registered PF_ALG protocol family
[    5.340393] macb 1f00100000.ethernet eth0: PHY [1f00100000.ethernet-ffffffff:01] driver [Broadcom BCM54213PE] (irq=POLL)
[    5.340403] macb 1f00100000.ethernet eth0: configuring for phy/rgmii-id link mode
[    5.343393] macb 1f00100000.ethernet: gem-ptp-timer ptp clock registered.
[    5.354445] brcmfmac: brcmf_cfg80211_set_power_mgmt: power save enabled
[    7.408260] Bluetooth: RFCOMM TTY layer initialized
[    7.408272] Bluetooth: RFCOMM socket layer initialized
[    7.408279] Bluetooth: RFCOMM ver 1.11
[  187.426189] dtsled: loading out-of-tree module taints kernel.
[  187.426897] dtsled dtsled@0: probed: /dev/myled ready
[  187.426958] dtsled: loaded
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ ls -l /dev/myled
crw------- 1 root root 509, 0 Sep  7 22:08 /dev/myled
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ echo 1 | sudo tee /dev/myled
1
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ echo 0 | sudo tee /dev/myled
0
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ gcc -O2 -Wall -o ledAPP ledAPP.c
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo ./ledAPP /dev/myled on
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo ./ledAPP /dev/myled off
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo ./ledAPP /dev/myled toggle
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo ./ledAPP /dev/myled read
1
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo ./ledAPP /dev/myled blink 500 500
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo ./ledAPP /dev/myled blink_stop
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ sudo ./ledAPP /dev/myled blink 500 500
a0000@mcalab:~/Desktop/linux_driver/class/class_20 $ 
```