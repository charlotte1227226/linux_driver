# 加載與卸載模組
## 1. 驅動模組（`.ko`）

* 當你把驅動程式原始碼編譯完成後，會產生一個副檔名為 `.ko`（kernel object）的二進位模組檔。
* 這個檔案就是要「插入」Linux 核心中執行的驅動程式。

---

## 2. 載入模組：`insmod` vs `modprobe`

| 指令         | 功能                                                             | 注意事項                                                                                        |
| ---------- | -------------------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| `insmod`   | 最簡單、最直接：**只**載入你指定的 `.ko` 檔。                                   | 不會自動處理依賴關係；若該模組依賴其他模組（例如 `drv.ko` 依賴 `first.ko`），必須手動先 `insmod first.ko` 再 `insmod drv.ko`。 |
| `modprobe` | 智能化：會先查 `/lib/modules/$(uname -r)/modules.dep`，**自動**載入所有依賴模組。 | 預設只在 `/lib/modules/<kernel-version>/` 目錄下搜尋，若你的根檔案系統中沒有這個目錄，就要手動建立並放入對應的 `.ko` 檔。           |

* **建議**：平常使用 `modprobe 驅動名`（不帶 `.ko`），它會做依賴分析、錯誤檢查，並自動載入所需的一切。

---

## 3. 卸載模組：`rmmod` vs `modprobe -r`

| 指令                | 功能                                         |
| ----------------- | ------------------------------------------ |
| `rmmod drv.ko`    | 最直接：**只**卸載你指定的 `.ko` 模組（可帶或不帶 `.ko` 副檔名）。 |
| `modprobe -r drv` | 智能卸載：會判斷哪些模組依賴該模組，連帶卸載不再使用的依賴模組。           |

* 使用 `modprobe -r` 可以避免手動一層層 `rmmod`，更方便地清理依賴關係。

---

### 小結流程範例

1. **編譯**：產生 `first.ko`、`drv.ko`。
2. **載入**：

   * 若用 `insmod drv.ko` → 會因找不到 `first.ko` 而失敗。
   * 建議：`modprobe first` → `modprobe drv`，或直接 `modprobe drv`（前提是 `modules.dep` 已正確建置）。
3. **卸載**：

   * 單獨卸載：`rmmod drv`。
   * 智能卸載：`modprobe -r drv` → 連帶卸載 `first`（若不再被其他模組使用）。

如此即可有效地管理核心模組的載入與卸載，而不必手動處理繁瑣的依賴關係。

---
以下是核心模組載入與卸載的指令教學（皆在終端下執行）：

```bash
# 1. 直接載入指定 .ko（不處理依賴）
sudo insmod first.ko
sudo insmod drv.ko

# 2. 智能載入（自動解析依賴）
sudo modprobe first
sudo modprobe drv

# 3. 直接卸載指定模組
sudo rmmod drv
sudo rmmod first

# 4. 智能卸載（連帶移除不再使用的依賴）
sudo modprobe -r drv
```

* `insmod` / `rmmod`：只對指定檔案動作，不處理依賴。
* `modprobe` / `modprobe -r`：自動處理依賴，推薦日常使用。

---

當你用 **modprobe** 載入模組時，它會到 `/lib/modules/$(uname -r)/` 目錄下去讀取 **modules.dep**（以及 modules.alias、modules.symbols）這些依賴檔，然後才能自動解析並載入所有依賴模組。如果這些檔案不存在，就會出現：

```
modprobe: can't open 'modules.dep': No such file or directory
```

也就是圖 40.4.4.2 的錯誤提示。

---

## 解決方法

1. **產生依賴檔**

   ```bash
   cd /lib/modules/$(uname -r)
   sudo depmod
   ```

   這條指令會自動掃描該目錄下所有 `.ko`，並生成：

   * `modules.dep`    （記錄模組之間的依賴關係）
   * `modules.alias`
   * `modules.symbols`

2. **再次執行 modprobe**

   ```bash
   sudo modprobe chrdevbase    # 不要帶 .ko 後綴
   ```

   這時候就不會再報找不到 `modules.dep` 了，模組也能正常載入。

---

### 如果系統沒有 `depmod`

* 有些精簡的根檔系統（例如只用 BusyBox）可能預設沒編進 `depmod`。
* 這時你需要：

  1. 重新編譯 BusyBox，把 **depmod** 這個 applet 打開（CONFIG\_DEPMOD）。
  2. 將新的 busybox 映像燒入系統。

如此一來，就能在 `/lib/modules/$(uname -r)` 下執行 `depmod`，讓 `modprobe` 正常工作了。

---
## 假設你的模組檔都放在「桌面」，路徑是 `~/Desktop/your_module.ko`，那麼：

---

### 1. 用 `insmod`（最簡單，不加工依賴）

```bash
cd ~/Desktop
sudo insmod your_module.ko
```

或直接帶絕對路徑：

```bash
sudo insmod /home/你的使用者名稱/Desktop/your_module.ko
```

> **重點**：`insmod` 只要路徑對，就能載入，不用搬到 `/lib/modules/...`。

---

### 2. 用 `modprobe` （要自動處理依賴，必須先搬檔）

如果你想用 `modprobe your_module`，必須先把 `.ko` 放到系統預設能搜尋到的模組目錄，然後再執行 `depmod -a`：

```bash
# 1) 把 .ko 複製到 /lib/modules/$(uname -r)/extra/
sudo cp ~/Desktop/your_module.ko /lib/modules/$(uname -r)/extra/

# 2) 重新產生 modules.dep 等依賴檔
sudo depmod -a

# 3) 呼叫 modprobe（不帶 .ko）
sudo modprobe your_module
```

> **提示**：若不想搬檔，就直接用 `insmod`；只要想要「自動解析依賴」，再把模組放進 `/lib/modules`、depmod 一下就行。
