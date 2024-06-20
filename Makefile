CC = gcc
CFLAGS = -std=c99 -Werror -Wall -Wextra $(shell pkg-config --cflags dbus-1 dbus-glib-1 glib-2.0)
LDFLAGS = $(shell pkg-config --libs dbus-1 dbus-glib-1 glib-2.0)
SRCDIR = src
BUILDDIR = build
SRC = $(wildcard $(SRCDIR)/*.c)
TARGETS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%,$(SRC))

all: $(TARGETS)

$(BUILDDIR)/%: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -rf $(BUILDDIR)

.PHONY: all clean
