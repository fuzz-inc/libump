include $(abspath $(dir $(lastword $(MAKEFILE_LIST))))/Makefile.def

PCH	:= $(SRCDIR)/ump/stdafx.hpp
CFLAGS	+= -I$(SRCDIR) -include $(PCH) -pthread 
LDFLAGS	+= -pthread

#
CFLAGS	+= $(shell pkg-config --cflags openssl)
LDFLAGS	+= $(shell pkg-config --libs openssl)

SRCDIRS	:= \
	ump \
	ump/client \
	ump/mj \
	ump/server \
	ump/socket \
	ump/thread

SRCS	:= $(foreach dir, $(SRCDIRS), $(wildcard $(SRCDIR)/$(dir)/*.cpp))
OBJS	:= $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%$(OBJEXT), $(SRCS))
TARGET	:= $(LIBDIR)/libump$(LIBEXT)

#
$(TARGET): $(OBJS)

#
$(OBJS): $(PCH)

-include $(patsubst %$(OBJEXT), %.d, $(OBJS))
