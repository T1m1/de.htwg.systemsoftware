##### LEDFLIP

LEDFLIP_VERSION = 1.0
LEDFLIP_SITE_METHOD = file
LEDFLIP_SITE = ledflip-$(LEDFLIP_VERSION).tar.gz


LEDFLIP_DEPENDENCIES = linux

define LEDFLIP_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define LEDFLIP_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/ledflip $(TARGET_DIR)/usr/bin/ledflip
endef

define LEDFLIP_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define LEDFLIP_UNINSTALL_TARGET_CMS
	rm $(TARGET_DIR)/usr/bin/access
endef

$(eval $(generic-package))
