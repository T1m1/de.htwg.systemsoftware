##### ZERO

ZERO_VERSION = 1.0
ZERO_SITE_METHOD = file
ZERO_SITE = zero-$(ZERO_VERSION).tar.gz


ZERO_DEPENDENCIES = linux

define ZERO_BUILD_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules 
endef

define ZERO_INSTALL_TARGET_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules_install
	$(INSTALL) -m 0755 -D $(@D)/test_myzero.sh $(TARGET_DIR)/usr/bin/test_myzero.sh
endef

define ZERO_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define ZERO_UNINSTALL_TARGET_CMS
	rm $(TARGET_DIR)/usr/bin/zero
endef

$(eval $(generic-package))
