include Makefile.def

#
PCH	= $(SRCDIR)/test/header.hpp
CFLAGS	+= -I$(SRCDIR) -include $(PCH)

#
SRCDIRS = \
	test

SRCS	= $(foreach dir, $(SRCDIRS), $(wildcard $(SRCDIR)/$(dir)/*.cpp))
OBJS	= $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%$(OBJEXT), $(SRCS))
TARGET	= $(BINDIR)/test$(BINEXT)
LIBUMP	= $(LIBDIR)/libump$(LIBEXT)

#
.PHONY: all
all: $(TARGET)
	$(TARGET)

#
$(TARGET): $(OBJS) $(LIBUMP)

#
.PHONY: $(LIBUMP)
$(LIBUMP):
	$(MAKE) -f libump.makefile

#
$(OBJS): $(PCH)

-include $(patsubst %$(OBJEXT), %.d, $(OBJS))
