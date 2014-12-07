##### ZERO

MYZERO_VERSION = 1.0
MYZERO_SITE_METHOD = file
MYZERO_SITE = myzero-$(MYZERO_VERSION).tar.gz


MYZERO_DEPENDENCIES = linux

define MYZERO_BUILD_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules 
endef

define MYZERO_INSTALL_TARGET_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules_install
	$(INSTALL) -m 0755 -D $(@D)/test_myzero.sh $(TARGET_DIR)/usr/bin/test_myzero.sh
endef

define MYZERO_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define ZERO_UNINSTALL_TARGET_CMS
	rm $(TARGET_DIR)/usr/bin/zero
endef

$(eval $(generic-package))
