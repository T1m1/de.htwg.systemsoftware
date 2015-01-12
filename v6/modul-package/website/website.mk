##### WEBSITE

WEBSITE_VERSION = 1.0
WEBSITE_SITE_METHOD = file
WEBSITE_SITE = website-$(WEBSITE_VERSION).tar.gz


WEBSITE_DEPENDENCIES = linux

define WEBSITE_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/index.html $(TARGET_DIR)/www/index.html
	$(INSTALL) -m 0755 -D $(@D)/main.js $(TARGET_DIR)/www/js/main.js
	$(INSTALL) -m 0755 -D $(@D)/led250.sh $(TARGET_DIR)/www/cgi-bin/ledrpi250.sh
	$(INSTALL) -m 0755 -D $(@D)/ledflip.sh $(TARGET_DIR)/www/cgi-bin/ledrpiflip.sh


endef


$(eval $(generic-package))
