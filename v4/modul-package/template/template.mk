##### TEMPLATE

TEMPLATE_VERSION = 1.0
TEMPLATE_SOURCE = template-$TEMPLATE_VERSION.tar.gz

define RUGOAPP_BUILD_CMS
	cd $(@D); \
		$(MAKE) both
endef

define RUGOAPP_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/app_static $(Target_DIR)/etc
	$(INSTALL) -D -m 0755 $(@D)/app_dynamica $(Target_DIR)/etc
endef

define RUGOAPP_UNINSTALL_TARGET_CMS
	rm $(TARGET_DIR)/etc/app_static
	rm $(TARGET_DIR)/etc/app_dynamic
endef

$(eval $(generic-package))
	
