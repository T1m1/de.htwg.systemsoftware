##### BUF

BUF_VERSION = 1.0
BUF_SITE_METHOD = file
BUF_SITE = buf-$(BUF_VERSION).tar.gz


BUF_DEPENDENCIES = linux

define BUF_BUILD_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules 
endef

define BUF_INSTALL_TARGET_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules_install
	$(INSTALL) -m 0755 -D $(@D)/test_buf.sh $(TARGET_DIR)/usr/bin/test_buf.sh
endef

define BUF_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define BUF_UNINSTALL_TARGET_CMS
	rm $(TARGET_DIR)/usr/bin/template
endef

$(eval $(generic-package))
