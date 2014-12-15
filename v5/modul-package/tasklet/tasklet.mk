##### TASKLET

TASKLET_VERSION = 1.0
TASKLET_SITE_METHOD = file
TASKLET_SITE = tasklet-$(TASKLET_VERSION).tar.gz


TASKLET_DEPENDENCIES = linux

define TASKLET_BUILD_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules 
endef

define TASKLET_INSTALL_TARGET_CMDS
	$(MAKE) $(LINUX_MAKE_FLAGS) -C $(LINUX_DIR) M=$(@D) modules_install
endef

define TASKLET_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

define TASKLET_UNINSTALL_TARGET_CMS
	rm $(TARGET_DIR)/usr/bin/template
endef

$(eval $(generic-package))
