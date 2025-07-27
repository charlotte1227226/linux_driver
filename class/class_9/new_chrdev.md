# 新的字符設備驅動原理
1. 以前的缺點
    - 使用`register_chrdev`函數註冊字符設備，浪費很多次設備號，而且需要我們手動指定主設備號
2. 新的
    - `int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name);`
3. 卸載驅動的時候使用`unregister_chrdev_region`函數，釋放前面申請的設備號，函數原型:
    - `void unregister_chrdev_region(dev_t dev, unsigned int count);`
4. 指定主設備號
    - 如果指定主設備號的話，那麼使用`register_chrdev_region`，函數原型是:
    - `int register_chrdev_region(dev_t from, unsigned int count, const char *name);`
    - 一般是給定主設備號，然後使用MKDEV構建完整的 `dev_t`，一般次設備號選擇 0 。


---
## 補充說明
> 在前兩章的實戰操作中，我們已經學會了使用老的字符設備驅動接口——透過 `register_chrdev` 註冊、`unregister_chrdev` 註銷設備，以及載入驅動後還要手動用 `mknod` 創建設備節點。
> 現在 Linux 內核推薦使用更新的字符設備驅動 API，不再直接調用上述兩個函數。本章將帶你學習如何使用這組新 API 編寫字符設備驅動，並在驅動模塊加載時自動創建對應的設備節點檔案。

---
這四個參數的意義如下（都在 `alloc_chrdev_region()` 執行時傳入）：

1. **`dev_t *dev`**

   * 這是一個指向 `dev_t`（整數型別，用來同時存放 major 和 minor 編號）的指標。
   * 呼叫成功後，`*dev` 會被填上「分配到的第一個設備號」（major 和基準 minor 組合而成），之後就可以用它來註冊你的字元設備。

2. **`unsigned baseminor`**

   * 指定你希望的「起始 minor 編號」。
   * 例如傳 0，就表示第一個設備的 minor 編號是 0，接下來的如果要分配多個（`count > 1`）就是 1、2、3… 依序排列。

3. **`unsigned count`**

   * 要一次性分配多少個連續的次設備號（minor numbers）。
   * 如果你只有一個字元設備，通常填 1；如果你的驅動要管理多個裝置（例如 `/dev/foo0`、`/dev/foo1`…），就把它設成你需要的數量。

4. **`const char *name`**

   * 這個字串會出現在 `/proc/devices` 列表中，作為你驅動的名字，也常被 udev 及其他使用者空間工具拿來做識別。
   * 一般慣例用你的驅動名或設備名，例如 `"mychardev"`。

---

**小結**：

```c
dev_t dev;
int ret = alloc_chrdev_region(&dev,    /* 取得的第一個設備號會寫到 dev */ 
                              0,       /* minor 從 0 開始 */ 
                              1,       /* 分配 1 個連續號 */ 
                              "mychardev");  /* 在 /proc/devices 中顯示的名稱 */
if (ret < 0) {
    /* 分配失敗，ret 會是負錯誤碼 */
}
```

---
```c
void unregister_chrdev_region(dev_t dev, unsigned int count);
```

* **`dev_t dev`**：要釋放的起始設備號（之前 `alloc_chrdev_region()` 回傳的那個值）。
* **`unsigned int count`**：要釋放的設備號範圍大小（跟 `alloc_chrdev_region()` 當初的 `count` 一致）。

當你在模組卸載函數（如 `__exit mydrv_exit(void)`）中呼叫它，就會將這段先前分配的 major/minor 編號返還給核心：

```c
static void __exit mydrv_exit(void)
{
    /* 移除 cdev 等其餘資源後… */
    unregister_chrdev_region(dev, 1);
}
```
