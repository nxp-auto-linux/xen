XEN_ROOT = $(CURDIR)/../..
include $(XEN_ROOT)/tools/Rules.mk

# Xen configuration dir and configs to go there.
XEN_READMES = README
XEN_READMES += README.incompatibilities

XEN_CONFIGS += xlexample.hvm
XEN_CONFIGS += xlexample.pvlinux
XEN_CONFIGS += xl.conf
XEN_CONFIGS += cpupool
XEN_CONFIGS += domU1.cfg domU2.cfg domU_mmc.cfg

XEN_CONFIGS += $(XEN_CONFIGS-y)

.PHONY: all
all:

.PHONY: build
build:

.PHONY: install
install: all install-readmes install-configs

.PHONY: uninstall
uninstall: uninstall-readmes uninstall-configs

.PHONY: install-readmes
install-readmes:
	[ -d $(DESTDIR)$(XEN_CONFIG_DIR) ] || \
		$(INSTALL_DIR) $(DESTDIR)$(XEN_CONFIG_DIR)
	set -e; for i in $(XEN_READMES); \
	    do [ -e $(DESTDIR)$(XEN_CONFIG_DIR)/$$i ] || \
	    $(INSTALL_DATA) $$i $(DESTDIR)$(XEN_CONFIG_DIR); \
	done

.PHONY: uninstall-readmes
uninstall-readmes:
	rm -f $(addprefix $(DESTDIR)$(XEN_CONFIG_DIR)/, $(XEN_READMES))

.PHONY: install-configs
install-configs: $(XEN_CONFIGS)
	[ -d $(DESTDIR)$(XEN_CONFIG_DIR) ] || \
		$(INSTALL_DIR) $(DESTDIR)$(XEN_CONFIG_DIR)
	[ -d $(DESTDIR)$(XEN_CONFIG_DIR)/auto ] || \
		$(INSTALL_DIR) $(DESTDIR)$(XEN_CONFIG_DIR)/auto
	set -e; for i in $(XEN_CONFIGS); \
	    do [ -e $(DESTDIR)$(XEN_CONFIG_DIR)/$$i ] || \
	    $(INSTALL_DATA) $$i $(DESTDIR)$(XEN_CONFIG_DIR); \
	done

.PHONY: uninstall-configs
uninstall-configs:
	rm -f $(addprefix $(DESTDIR)$(XEN_CONFIG_DIR)/, $(XEN_CONFIGS))

.PHONY: clean
clean:

.PHONY: distclean
distclean: clean
