#
#
#
include $(UMPDIR)/build/linux/Makefile.def

CFLAGS	+= -I$(UMPDIR)

#
CFLAGS	+= $(shell pkg-config --cflags cppunit)
LDFLAGS	+= $(shell pkg-config --libs cppunit)

#
SRCDIRS = \
	test

SRCS	= $(foreach dir, $(SRCDIRS), $(wildcard $(UMPDIR)/$(dir)/*.cpp))
OBJS	= $(patsubst $(UMPDIR)/%.cpp, $(OBJDIR)/%$(OBJEXT), $(SRCS))
TARGET	= $(BINDIR)/umptest$(EXEEXT)

#
#
#
.PHONY: run
run: build
	$(TARGET)

#
#
#
.PHONY: build
build: $(TARGET)

$(TARGET): $(OBJS) $(UMPDIR)/build/linux/$(LIBDIR)/libump.a

-include $(patsubst %$(OBJEXT), %.d, $(OBJS))

#
#
#
.PHONY: clean
clean:
	$(RM) $(OBJDIR) $(TARGET)
