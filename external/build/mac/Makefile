CURDIR	:= $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
SRCDIR	:= $(abspath $(CURDIR)/../../openssl)
OPTDIR	:= $(abspath $(CURDIR)/../../opt/mac/openssl)

.PHONY: all
all:
	rm -rf $(OPTDIR)
	cd $(SRCDIR); \
	./Configure --prefix=$(OPTDIR) darwin64-x86_64-cc; \
	make clean; \
	make MACOSX_DEPLOYMENT_TARGET=10.9; \
	make install_sw
