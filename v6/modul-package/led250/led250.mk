##### LED250

LED250_VERSION = 1.0
LED250_SITE_METHOD = file
LED250_SITE = led250-$(LED250_VERSION).tar.gz


LED250_DEPENDENCIES = linux

define LED250_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define LED250_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/led250 $(TARGET_DIR)/usr/bin/led250
	$(INSTALL) -m 0755 -D $(@D)/test_led250.sh $(TARGET_DIR)/usr/bin/test_ledrpi250.sh
	
endef

$(eval $(generic-package))
