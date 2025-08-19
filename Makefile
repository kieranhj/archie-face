###############################################################################
# Setting up ArchieSDK
ifeq ($(strip $(ARCHIESDK)),)
$(error "ARCHIESDK was not found in your environment. please export ARCHIESDK)
endif
include $(ARCHIESDK)/config.mk
###############################################################################

ARCHIEOBJDUMP=$(ARCHIESDK)/tools/bin/arm-archie-objdump

# Your program's name
APPNAME = grid

LIBFILES = lib/debug.c lib/mem.c lib/mouse.c lib/plot.c lib/trig.c lib/video.c

# Your source files (.c or .s)
SRCFILES = main.c src/flow-field.c

# Extra libraries
LIBS = -lm

# Make sure to always append to CFLAGS with += instead of overwriting them
# Add -g to interleave source with asm in compile.txt
# -fno-zero-initialized-in-bss was a workaround for BSS not being initialise to zero.
CFLAGS += -O2 -g

all: build
# Build program
	$(ARCHIECC) $(CFLAGS) -obuild/$(APPNAME).elf $(SRCFILES) $(LIBFILES) $(LIBS) 
# Get a human readable output of the assembly and symbols etc.
	 $(ARCHIEOBJDUMP) -x -d -S -t build/$(APPNAME).elf > build/compile.txt
# Extract final binary from ELF
	$(ARCHIEOBJCOPY) -O binary build/$(APPNAME).elf build/$(APPNAME),ff8
# Copy final binary to hostfs (modify config.mk to set your path)
	cp -f build/$(APPNAME),ff8 $(ARCHIECOPYPATH)/$(APPNAME),ff8 | true

build:
	mkdir build

asm:
# Generate ASM from C files
	$(ARCHIECC) $(CFLAGS) -S $(SRCFILES) $(LIBFILES)

clean:
# Clean up build files
	rm -f *,ff8
	rm -f tmpProg
