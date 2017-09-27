###############################################################################
# XMBurner for Uzebox
###############################################################################
#
#  Copyright (C) 2017
#    Sandor Zsuga (Jubatian)
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#
#
# The main makefile of the program
#
#
# make all (or make): build the program
# make clean:         to clean up
#
#

## Paths to Uzebox components & XMBurner
UZEBOX_DIR   = ../../../uze_src/uzebox
XMBURNER_DIR = ../xmburner

## Uzebox components
KERNEL_DIR   = $(UZEBOX_DIR)/kernel
PACKROM_PATH = $(UZEBOX_DIR)/bin/packrom

## Binary size (for appending CRC)
BINSIZE      = 61440


## General Flags
PROJECT = uzeburn
GAME    = uzeburn
INFO    = gameinfo.properties
MCU     = atmega644
TARGET  = $(GAME).elf
CC      = avr-gcc
OUTDIR  = _bin_
OBJDIR  = _obj_
DEPDIR  = _dep_
DIRS    = $(OUTDIR) $(OBJDIR) $(DEPDIR)

## Kernel settings
KERNEL_OPTIONS  = -DVIDEO_MODE=90
KERNEL_OPTIONS += -DSOUND_MIXER=MIXER_TYPE_INLINE
KERNEL_OPTIONS += -DCENTER_ADJUSTMENT=18


## Options common to compile, link and assembly rules
COMMON  = -mmcu=$(MCU)
COMMON += -DBINSIZE=$(BINSIZE)

## Compile options common for all C compilation units.
CFLAGS  = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=28636360UL -O2 -fsigned-char
CFLAGS += -ffunction-sections -fno-toplevel-reorder -fno-tree-switch-conversion
CFLAGS += -MD -MP -MT $(*F).o -MF $(DEPDIR)/$(@F).d
CFLAGS += $(KERNEL_OPTIONS)


## Assembly specific flags
ASMFLAGS  = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS  = $(COMMON)
LDFLAGS += -Wl,-Map=$(OUTDIR)/$(GAME).map
LDFLAGS += -Wl,-gc-sections

## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom


## XMBurner definitions
XMB_CC    = $(CC)
XMB_MCU   = $(MCU)
XMB_NATCC = gcc
XMB_SPATH = $(XMBURNER_DIR)
XMB_OPATH = .
include $(XMBURNER_DIR)/xmb_defs.mk
DIRS     += $(XMB_OBJ)
DIRS     += $(XMB_BIN)


## Objects that must be built in order to link
OBJECTS  = $(OBJDIR)/uzeboxVideoEngineCore.o
OBJECTS += $(OBJDIR)/uzeboxCore.o
OBJECTS += $(OBJDIR)/uzeboxSoundEngine.o
OBJECTS += $(OBJDIR)/uzeboxSoundEngineCore.o
OBJECTS += $(OBJDIR)/uzeboxVideoEngine.o
OBJECTS += $(OBJDIR)/$(GAME).o
OBJECTS += $(OBJDIR)/xmb_if.o
OBJECTS += $(OBJDIR)/tileset.o

## Include Directories
INCLUDES  = -I"$(KERNEL_DIR)"
INCLUDES += -I"$(XMBURNER_DIR)/xmburner"

## Build
all: $(OUTDIR)/$(TARGET) $(OUTDIR)/$(GAME).hex $(OUTDIR)/$(GAME).lss $(OUTDIR)/$(GAME).uze size

## Directories
$(OBJDIR):
	mkdir $(OBJDIR)

$(OUTDIR):
	mkdir $(OUTDIR)

$(DEPDIR):
	mkdir $(DEPDIR)

## Compile XMBurner
include $(XMBURNER_DIR)/xmb_ruls.mk

## Compile Kernel files
$(OBJDIR)/uzeboxVideoEngineCore.o: $(KERNEL_DIR)/uzeboxVideoEngineCore.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

$(OBJDIR)/uzeboxSoundEngineCore.o: $(KERNEL_DIR)/uzeboxSoundEngineCore.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

$(OBJDIR)/uzeboxCore.o: $(KERNEL_DIR)/uzeboxCore.c $(DIRS)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(OBJDIR)/uzeboxSoundEngine.o: $(KERNEL_DIR)/uzeboxSoundEngine.c $(DIRS)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(OBJDIR)/uzeboxVideoEngine.o: $(KERNEL_DIR)/uzeboxVideoEngine.c $(DIRS)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

## Compile game sources
$(OBJDIR)/$(GAME).o: main.c $(DIRS)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(OBJDIR)/xmb_if.o: xmb_if.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

$(OBJDIR)/tileset.o: tileset.s $(DIRS)
	$(CC) $(INCLUDES) $(ASMFLAGS) -c $< -o $@

##Link
$(OUTDIR)/$(TARGET): $(OBJECTS) $(DIRS) $(XMB_OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(XMB_OBJECTS) $(LIBDIRS) $(LIBS) -o $(OUTDIR)/$(TARGET)

$(OUTDIR)/%.hex: $(OUTDIR)/%.hexraw $(XMB_CRCHEX_BIN)
	$(XMB_CRCHEX_BIN) $< $(BINSIZE) > $@

$(OUTDIR)/%.hexraw: $(OUTDIR)/$(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS) $< $@

$(OUTDIR)/%.lss: $(OUTDIR)/$(TARGET)
	avr-objdump -h -S $< > $@

$(OUTDIR)/%.uze: $(OUTDIR)/$(TARGET)
	$(PACKROM_PATH) $(OUTDIR)/$(GAME).hex $@ $(INFO)

UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')
AVRSIZEFLAGS := -A $(OUTDIR)/${TARGET}
ifneq (,$(findstring MINGW,$(UNAME)))
AVRSIZEFLAGS := -C --mcu=${MCU} $(OUTDIR)/${TARGET}
endif

size: $(OUTDIR)/${TARGET}
	@echo
	@avr-size ${AVRSIZEFLAGS}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(DIRS)

