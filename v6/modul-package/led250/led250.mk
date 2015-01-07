##### LED250

LED250_VERSION = 1.0
LED250_SITE_METHOD = file
LED250_SITE = led250-$(LED250_VERSION).tar.gz


LED250_DEPENDENCIES = linux

define LED250_BUILD_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules 
endef

define LED250_INSTALL_TARGET_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules_install
	$(INSTALL) -m 0755 -D $(@D)/test_mygpio.sh $(TARGET_DIR)/usr/bin/test_mygpio.sh
endef

define LED250_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define LED250_UNINSTALL_TARGET_CMS
	rm $(TARGET_DIR)/usr/bin/template
endef

$(eval $(generic-package))
