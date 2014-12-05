##### NULL

MYNULL_VERSION = 1.0
MYNULL_SITE_METHOD = file
MYNULL_SITE = mynull-$(MYNULL_VERSION).tar.gz


MYNULL_DEPENDENCIES = linux

define MYNULL_BUILD_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules 
endef

define MYNULL_INSTALL_TARGET_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules_install
endef

define MYNULL_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define MYNULL_UNINSTALL_TARGET_CMS
	rm $(TARGET_DIR)/usr/bin/mynull
endef

$(eval $(generic-package))
