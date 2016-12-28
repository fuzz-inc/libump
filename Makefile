PLATFORM	= linux
MAKEFLAGS	= UMPDIR=$(CURDIR)

include build/$(PLATFORM)/Makefile.def

BUILD_DIRS = \
	build/$(PLATFORM) \
	sample/server/build/$(PLATFORM) \
	sample/client/build/$(PLATFORM)

DEBUGGER	= 

#
#
#
TARGETS = build clean
.PHONY: $(TARGETS)
$(TARGETS):
	@for dir in $(BUILD_DIRS); do \
		$(MAKE) -C $$dir $@; \
	done

#
#
#
.PHONY: test
test: build
	$(MAKE) -C test/build/$(PLATFORM)

#
#
#
.PHONY: server
server: build
	$(DEBUGGER) sample/server/build/$(PLATFORM)/$(BINDIR)/umpserver$(EXEEXT)

#
#
#
.PHONY: client
client: build
	cd sample/client; \
	$(DEBUGGER) build/$(PLATFORM)/$(BINDIR)/umpclient$(EXEEXT)
