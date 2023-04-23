SOURCEDIR=./src
BUILDDIR=./build
EXECUTABLE:=$(BUILDDIR)/basketballer
RUNCMD ?= $<

CFLAGS := $(CFLAGS) -MD -Iinclude $(shell pkg-config sdl2 --cflags)
LDFLAGS := $(LDFLAGS) $(shell pkg-config sdl2 --libs)

SOURCES=$(wildcard $(SOURCEDIR)/*.c)
OBJECTS=$(patsubst %.c,$(BUILDDIR)/%.o,$(notdir $(SOURCES)))

$(EXECUTABLE): $(OBJECTS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) $(OUTPUT_OPTION)

$(OBJECTS): | $(BUILDDIR)

$(BUILDDIR):
	mkdir -p $@

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

-include $(OBJECTS:.o=.d)

.PHONY: run clean purge
run: $(EXECUTABLE)
	$(RUNCMD)
clean:
	rm -f $(BUILDDIR)/*.o $(BUILDDIR)/*.d
purge:
	rm -rf $(BUILDDIR)
