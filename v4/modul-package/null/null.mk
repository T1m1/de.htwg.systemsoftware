##### NULL

NULL_VERSION = 1.0
NULL_SITE_METHOD = file
NULL_SITE = NULL-$(NULL_VERSION).tar.gz


NULL_DEPENDENCIES = linux

define NULL_BUILD_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules 
endef

define NULL_INSTALL_TARGET_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules_install
endef

define NULL_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define NULL_UNINSTALL_TARGET_CMS
	rm $(TARGET_DIR)/usr/bin/null
endef

$(eval $(generic-package))
