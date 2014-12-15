##### TIMER

TIMER_VERSION = 1.0
TIMER_SITE_METHOD = file
TIMER_SITE = timer-$(TIMER_VERSION).tar.gz


TIMER_DEPENDENCIES = linux

define TIMER_BUILD_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules 
endef

define TIMER_INSTALL_TARGET_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules_install
	$(INSTALL) -m 0755 -D $(@D)/test_timer.sh $(TARGET_DIR)/usr/bin/test_timer.sh
endef

define TIMER_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define TIMER_UNINSTALL_TARGET_CMS
	rm $(TARGET_DIR)/usr/bin/timer
endef

$(eval $(generic-package))
