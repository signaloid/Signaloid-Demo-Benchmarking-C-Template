#
#	Makefile for native Monte Carlo builds with Signaloid UxHw support.
#
#	Copyright (c) 2026, Signaloid.
#
#	Targets:
#	  local-build        - Build for native execution with the GSL compat layer
#	  local-run          - Build and run Monte Carlo for output 0
#	  clean              - Remove build artifacts
#

#
#	Include `config.mk` first, so that the variables it sets cannot
#	overwrite the ones this Makefile defines below. `config.mk` is the
#	source list shared with the Signaloid cloud build.
#
include src/config.mk

CC = gcc

#
#	Use `gnu11` rather than `c11`: the UxHw compatibility shim calls the
#	POSIX `random()` and `srandom()`, which a strict ISO C dialect does
#	not declare.
#
CFLAGS = -std=gnu11 -Wall -Wextra -O2 -Isrc
LDFLAGS = -lm -lgsl -lgslcblas

#
#	Platform-specific paths. On macOS, GSL is not on the default search
#	path, so locate it under MacPorts (/opt/local), Homebrew on Apple
#	Silicon (/opt/homebrew) or Homebrew on Intel (/usr/local).
#
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    ifneq ($(wildcard /opt/local/include/gsl),)
        CFLAGS  += -I/opt/local/include
        LDFLAGS += -L/opt/local/lib
    else ifneq ($(wildcard /opt/homebrew/include/gsl),)
        CFLAGS  += -I/opt/homebrew/include
        LDFLAGS += -L/opt/homebrew/lib
    else ifneq ($(wildcard /usr/local/include/gsl),)
        CFLAGS  += -I/usr/local/include
        LDFLAGS += -L/usr/local/lib
    endif
endif

SRCDIR = src
BUILDDIR = build
TARGET = demo-native-mc

#
#	`config.mk` omits `uxhw.c`, because the Signaloid cloud compiler
#	provides the UxHw API natively. The native Monte Carlo build has to
#	compile the compatibility shim in explicitly.
#
SOURCES := $(addprefix $(SRCDIR)/,$(SOURCES)) $(SRCDIR)/uxhw.c

OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

.PHONY: all build-local local-build run-local local-run clean

all: build-local

build-local: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

run-local: build-local
	./$(TARGET) -M 10000 -S 0 -T

local-build: build-local

local-run: run-local

clean:
	rm -rf $(BUILDDIR) $(TARGET)

