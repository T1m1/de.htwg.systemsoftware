##### KTHREAD

KTHREAD_VERSION = 1.0
KTHREAD_SITE_METHOD = file
KTHREAD_SITE = kthread-$(KTHREAD_VERSION).tar.gz


KTHREAD_DEPENDENCIES = linux

define KTHREAD_BUILD_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules 
endef

define KTHREAD_INSTALL_TARGET_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules_install
	$(INSTALL) -m 0755 -D $(@D)/test_kthread.sh $(TARGET_DIR)/usr/bin/test_kthread.sh
endef

define KTHREAD_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define KTHREAD_UNINSTALL_TARGET_CMS
	rm $(TARGET_DIR)/usr/bin/template
endef

$(eval $(generic-package))
