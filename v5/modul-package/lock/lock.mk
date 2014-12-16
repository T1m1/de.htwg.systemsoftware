##### LOCK

LOCK_VERSION = 1.0
LOCK_SITE_METHOD = file
LOCK_SITE = lock-$(LOCK_VERSION).tar.gz


LOCK_DEPENDENCIES = linux

define LOCK_BUILD_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules 
endef

define LOCK_INSTALL_TARGET_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules_install
	$(INSTALL) -m 0755 -D $(@D)/test_lock.sh $(TARGET_DIR)/usr/bin/test_lock.sh
endef

define LOCK_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define LOCK_UNINSTALL_TARGET_CMS
	rm $(TARGET_DIR)/usr/bin/lock
endef

$(eval $(generic-package))
