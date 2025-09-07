#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x9fa7184a, "cancel_delayed_work_sync" },
	{ 0x5630b473, "device_destroy" },
	{ 0x607587f4, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xf311fc60, "class_create" },
	{ 0xf970cbeb, "__platform_driver_register" },
	{ 0x122c3a7e, "_printk" },
	{ 0x4a41ecb3, "class_destroy" },
	{ 0x7eb87600, "devm_kmalloc" },
	{ 0xcefb0c9f, "__mutex_init" },
	{ 0xffeedf6a, "delayed_work_timer_fn" },
	{ 0xc6f46339, "init_timer_key" },
	{ 0x1a233f66, "of_property_read_string" },
	{ 0x476b165a, "sized_strscpy" },
	{ 0x832481e, "devm_gpiod_get" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x5d9d9fd4, "cdev_init" },
	{ 0xcc335c1c, "cdev_add" },
	{ 0x278fff66, "device_create" },
	{ 0x56787661, "_dev_info" },
	{ 0xe8243ddd, "_dev_err" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x25e47ffd, "platform_driver_unregister" },
	{ 0xdcb764ad, "memset" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0x4dfa8d4b, "mutex_lock" },
	{ 0xa59f831f, "gpiod_set_value_cansleep" },
	{ 0x3213f038, "mutex_unlock" },
	{ 0xbcab6ee6, "sscanf" },
	{ 0x7f02188f, "__msecs_to_jiffies" },
	{ 0x2d3385d3, "system_wq" },
	{ 0xb2fcb56d, "queue_delayed_work_on" },
	{ 0x96848186, "scnprintf" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x7682ba4e, "__copy_overflow" },
	{ 0x39ff040a, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Cmcalab,newchrled");
MODULE_ALIAS("of:N*T*Cmcalab,newchrledC*");

MODULE_INFO(srcversion, "54467B84703232726252B29");
