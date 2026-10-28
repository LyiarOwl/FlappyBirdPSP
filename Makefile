TARGET = psp_game
PSP_EBOOT_TITLE = psp_game

# ---------------------------------------------------------------------------
# Sources: every .c under src/, at any depth (main.c, module/*.c, ...)
# ---------------------------------------------------------------------------
SRC_DIR = src
SRCS    = $(shell find $(SRC_DIR) -type f -name '*.c' | sort)
OBJS    = $(SRCS:.c=.o)

# Include paths: src/ itself (so you can write #include "module/some2.h")
# plus every subdirectory of src/ (so you can also write #include "some2.h").
# build.mak turns each INCDIR entry into -I<entry>.
INCDIR  = $(shell find $(SRC_DIR) -type d | sort)

CFLAGS   = -O2 -G0 -Wall -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS  = $(CFLAGS)

# build.mak already links pspdebug, pspdisplay, pspge, pspctrl and pspsdk.
LIBDIR  =
LIBS    = -lpspgum -lpspgu -lpspfpu -lm
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP

PSPSDK = $(shell psp-config --pspsdk-path)

PSP_LARGE_MEMORY = 0

include $(PSPSDK)/lib/build.mak

# ---------------------------------------------------------------------------
# Extras
# ---------------------------------------------------------------------------
# `make dist` -> dist/psp_game/{EBOOT.PBP,assets/}, ready to copy to
# ms0:/PSP/GAME/ on the memory stick (or to point PPSSPP at).
DIST_DIR = dist/$(TARGET)

.PHONY: dist
dist: EBOOT.PBP
	mkdir -p $(DIST_DIR)
	cp EBOOT.PBP $(DIST_DIR)/
	rm -rf $(DIST_DIR)/assets
	cp -r assets $(DIST_DIR)/assets

# Also clean the generated EBOOT/PRX/ELF leftovers and dist/.
distclean: clean
	rm -rf dist PARAM.SFO EBOOT.PBP $(TARGET).elf $(TARGET).prx
