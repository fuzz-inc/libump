include Makefile.def

CFLAGS	+= -I$(SRCDIR) -pthread
LDFLAGS	+= -pthread

LIBUMP	= $(LIBDIR)/libump$(LIBEXT)

#
$(BINDIR)/umpserver: $(OBJDIR)/server/main$(OBJEXT) $(LIBUMP)

#
.PHONY: $(LIBUMP)
$(LIBUMP):
	$(MAKE) -f libump.makefile

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
