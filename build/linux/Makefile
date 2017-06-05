include $(abspath $(dir $(lastword $(MAKEFILE_LIST))))/Makefile.def

CFLAGS	+= -I$(SRCDIR) -pthread
LDFLAGS	+= -pthread

LIBUMP	= $(LIBDIR)/libump$(LIBEXT)

#
#
#
.PHONY: libump $(LIBUMP)

libump: $(LIBUMP)

$(LIBUMP):
	$(MAKE) -f libump.makefile

#
$(BINDIR)/umpserver: $(OBJDIR)/server/main$(OBJEXT) $(LIBUMP)

#
.PHONY: test
test:
	$(MAKE) -f test.makefile

#
.PHONY: clean
clean:
	$(RM) $(OBJDIR) $(LIBDIR) $(BINDIR)

#
$(OBJS): $(PCH)

-include $(patsubst %$(OBJEXT), %.d, $(OBJS))
