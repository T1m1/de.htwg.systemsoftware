##### TEMPLATE

TEMPLATE_VERSION = 1.0
TEMPLATE_SITE_METHOD = file
TEMPLATE_SITE = template-$(TEMPLATE_VERSION).tar.gz


TEMPLATE_DEPENDENCIES = linux

define TEMPLATE_BUILD_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules 
endef

define TEMPLATE_INSTALL_TARGET_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules_install
endef

define TEMPLATE_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define TEMPLATE_UNINSTALL_TARGET_CMS
	rm $(TARGET_DIR)/usr/bin/template
endef

$(eval $(generic-package))
