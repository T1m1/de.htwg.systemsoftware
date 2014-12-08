##### SYSTEMINFO

SYSTEMINFO_VERSION = 1.0
SYSTEMINFO_SITE_METHOD = file
SYSTEMINFO_SITE = systeminfo-$(SYSTEMINFO_VERSION).tar.gz


SYSTEMINFO_DEPENDENCIES = linux

define SYSTEMINFO_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define SYSTEMINFO_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/systeminfo $(TARGET_DIR)/usr/bin/systeminfo
endef

define SYSTEMINFO_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define SYSTEMINFO_UNINSTALL_TARGET_CMS
	rm $(TARGET_DIR)/usr/bin/systeminfo
endef

$(eval $(generic-package))
