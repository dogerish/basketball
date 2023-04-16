SOURCEDIR=./src
BUILDDIR=./build
EXECUTABLE:=$(BUILDDIR)/basketballer
# EXEC_CMD ?= 
# EXEC_ARGV ?= 

CFLAGS := $(CFLAGS) -MD -Iinclude $(shell pkg-config sdl2 --cflags)
LDFLAGS := $(LDFLAGS) $(shell pkg-config sdl2 --libs)

SOURCES=$(wildcard $(SOURCEDIR)/*.c)
OBJECTS=$(patsubst %.c,$(BUILDDIR)/%.o,$(notdir $(SOURCES)))

.PHONY: all run
all: $(EXECUTABLE)
run: $(EXECUTABLE)
	exec $(EXEC_CMD) $(EXECUTABLE) $(EXEC_ARGV)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $(EXECUTABLE)

$(OBJECTS): | $(BUILDDIR)

$(BUILDDIR):
	mkdir $@

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(OBJECTS:.o=.d)

.PHONY: clean purge
clean:
	rm $(OBJECTS)
purge:
	rm -rf $(BUILDDIR)
