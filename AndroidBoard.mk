LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_PREBUILT_KERNEL),)
TARGET_PREBUILT_KERNEL := $(LOCAL_PATH)/kernel
endif

file := $(INSTALLED_KERNEL_TARGET)
ALL_PREBUILT += $(file)
$(file): $(TARGET_PREBUILT_KERNEL) | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_RECOVERY_ROOT_OUT)/sbin/postrecoveryboot.sh
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/postrecoveryboot.sh | $(ACP)
	$(transform-prebuilt-to-target)
file := $(TARGET_RECOVERY_ROOT_OUT)/sbin/killrecovery.sh
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/killrecovery.sh | $(ACP)
	$(transform-prebuilt-to-target)
file := $(TARGET_RECOVERY_ROOT_OUT)/sbin/nandroid-md5.sh
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/nandroid-md5.sh | $(ACP)
	$(transform-prebuilt-to-target)

# link to hijack!
file := $(TARGET_OUT)/bin/logwrapper
ALL_PREBUILT += $(file)
$(file) : $(TARGET_OUT)/bin/hijack
	@echo "Symlink: $@ -> hijack"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf hijack $@
