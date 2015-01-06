##### MYGPIO

MYGPIO_VERSION = 1.0
MYGPIO_SITE_METHOD = file
MYGPIO_SITE = mygpio-$(MYGPIO_VERSION).tar.gz


MYGPIO_DEPENDENCIES = linux

define MYGPIO_BUILD_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules 
endef

define MYGPIO_INSTALL_TARGET_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules_install
	$(INSTALL) -m 0755 -D $(@D)/test_mygpio.sh $(TARGET_DIR)/usr/bin/test_mygpio.sh
endef

define MYGPIO_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define MYGPIO_UNINSTALL_TARGET_CMS
	rm $(TARGET_DIR)/usr/bin/template
endef

$(eval $(generic-package))
