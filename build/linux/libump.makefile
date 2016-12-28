include Makefile.def

PCH	:= $(SRCDIR)/ump/header.hpp
CFLAGS	+= -I$(SRCDIR) -include $(PCH) -pthread
LDFLAGS	+= -pthread

SRCDIRS	:= \
	ump \
	ump/client \
	ump/mj \
	ump/server \
	ump/socket

SRCS	:= $(foreach dir, $(SRCDIRS), $(wildcard $(SRCDIR)/$(dir)/*.cpp))
OBJS	:= $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%$(OBJEXT), $(SRCS))
TARGET	:= $(LIBDIR)/libump$(LIBEXT)

#
$(TARGET): $(OBJS)

#
$(OBJS): $(PCH)

-include $(patsubst %$(OBJEXT), %.d, $(OBJS))
