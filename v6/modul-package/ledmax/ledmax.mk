##### LEDMAX

LEDMAX_VERSION = 1.0
LEDMAX_SITE_METHOD = file
LEDMAX_SITE = ledmax-$(LEDMAX_VERSION).tar.gz


LEDMAX_DEPENDENCIES = linux

define LEDMAX_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define LEDMAX_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/ledmax $(TARGET_DIR)/usr/bin/ledmax
	$(INSTALL) -m 0755 -D $(@D)/test_ledmax.sh $(TARGET_DIR)/usr/bin/test_ledrpimax.sh
endef

define LEDMAX_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define LEDMAX_UNINSTALL_TARGET_CMS
	rm $(TARGET_DIR)/usr/bin/ledmax
endef

$(eval $(generic-package))
