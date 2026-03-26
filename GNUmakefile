# openiBoot GNUmakefile
# Usage: make iPhone2G | iPhone3G | iPodTouch1G | iPodTouch2G |
#              iPhone3GS | iPhone4 | iPad1G | iPodTouch4G | aTV2G

CROSS    ?= arm-none-eabi-
CC       := $(CROSS)gcc
OBJCOPY  := $(CROSS)objcopy
HOST_CC  := cc

VERSION    := 0.3
GIT_COMMIT := $(shell git log --format=%h -1)

BUILD := build

# ── ARM compiler flags ────────────────────────────────────────────────────────

PLAT_FLAGS := \
	-mlittle-endian -mfpu=vfp -mthumb -mthumb-interwork -std=gnu11

CFLAGS := $(PLAT_FLAGS) -nostdlib -Wall -O1 -fcommon \
	-DOPENIBOOT_VERSION=$(VERSION) \
	-DOPENIBOOT_VERSION_BUILD=$(GIT_COMMIT) \
	-DCONFIG_ARM

ASFLAGS := $(PLAT_FLAGS) -nostdlib -xassembler-with-cpp

LDFLAGS := $(PLAT_FLAGS) -nostdlib -Ttext=0x0

INCLUDES := -Iincludes -Iarch-arm/includes

# ── Host tools ────────────────────────────────────────────────────────────────

BIN2C     := images/bin2c
MK8900IMG := mk8900image/mk8900image

$(BIN2C): images/bin2c.c
	$(HOST_CC) -o $@ $<

# On Darwin arm64 the pre-built x86_64 binary runs via Rosetta.
# On other platforms, build from source.
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)
ifeq ($(UNAME_S) $(UNAME_M),Darwin arm64)
$(MK8900IMG):
	@test -f $@ || (echo "ERROR: mk8900image binary missing from mk8900image/"; exit 1)
else
MK8900_LIBS_Darwin      := mk8900image/mac-x86/libxpwn.a mk8900image/mac-x86/libcommon.a
MK8900_LIBS_Linux_x86_64 := mk8900image/x86_64/libxpwn.a mk8900image/x86_64/libcommon.a
MK8900_LIBS_Linux       := mk8900image/x86/libxpwn.a mk8900image/x86/libcommon.a
MK8900_LIBS := $(or $(MK8900_LIBS_$(UNAME_S)_$(UNAME_M)),$(MK8900_LIBS_$(UNAME_S)))
$(MK8900IMG): mk8900image/mk8900image.c $(MK8900_LIBS)
	$(HOST_CC) -no-pie -o $@ $< $(MK8900_LIBS) -L/usr/X11/lib -lm -ldl -lpng -lcrypto -lz
endif

# ── Source groups ─────────────────────────────────────────────────────────────

ARCH_ARM_SRC := \
	arch-arm/entry.sx \
	arch-arm/asmhelpers.sx \
	arch-arm/arm.c

BASE_SRC := \
	device.c mtd.c bdev.c nand.c vfl.c ftl.c audiohw.c actions.c \
	commands.c framebuffer.c images.c malloc.c nvram.c openiboot.c \
	printf.c scripting.c sha1.c stb_image.c syscfg.c tasks.c \
	util.c aes_wrap.c

# s5l8720 uses a subset of base_src (no audiohw, nand, vfl, ftl, aes_wrap)
S5L8720_BASE_SRC := \
	device.c mtd.c bdev.c actions.c commands.c framebuffer.c images.c \
	malloc.c nvram.c openiboot.c printf.c scripting.c sha1.c stb_image.c \
	syscfg.c tasks.c util.c

HFS_SRC := \
	hfs/bdev.c hfs/btree.c hfs/catalog.c hfs/extents.c \
	hfs/fastunicodecompare.c hfs/fs.c hfs/rawfile.c hfs/utility.c \
	hfs/volume.c hfs/xattr.c hfs/hfscompress.c

# ── Module sources and flags ──────────────────────────────────────────────────

ACM_SRC       := acm/acm.c
ACM_INC       := -Iacm/includes

USB_SRC       := usb-synopsys/usb.c
USB_INC       := -Iusb-synopsys/includes

NOR_CFI_SRC   := nor-cfi/nor.c
NOR_CFI_INC   := -Inor-cfi/includes

NOR_SPI_SRC   := nor-spi/nor.c
NOR_SPI_INC   := -Inor-spi/includes

VFL_VFL_SRC   := vfl-vfl/vfl.c
VFL_VFL_INC   := -Ivfl-vfl/includes -DCONFIG_VFL_VFL

VFL_VSVFL_SRC := vfl-vsvfl/vsvfl.c
VFL_VSVFL_INC := -Ivfl-vsvfl/includes -DCONFIG_VFL_VSVFL

FTL_YAFTL_SRC := ftl-yaftl/yaftl_mem.c ftl-yaftl/yaftl_common.c \
                 ftl-yaftl/yaftl.c ftl-yaftl/yaftl_gc.c ftl-yaftl/l2v.c
FTL_YAFTL_INC := -Iftl-yaftl/includes -DCONFIG_FTL_YAFTL

PMB8876_SRC   := radio-pmb8876/radio.c
PMB8876_INC   := -Iradio-pmb8876/includes

PMB8878_SRC   := radio-pmb8878/radio.c
PMB8878_INC   := -Iradio-pmb8878/includes

XGOLD618_SRC  := radio-xgold618/radio.c
XGOLD618_INC  := -Iradio-xgold618/includes

MENU_SRC      := menu/menu.c
MENU_INC      := -Imenu/includes -Imenu

INSTALLER_SRC := installer/installer.c
INSTALLER_INC := -Iinstaller

INSTALLER_IMGS := \
	installer/images/installerLogoPNG.h \
	installer/images/installerBarEmptyPNG.h \
	installer/images/installerBarFullPNG.h

# ── Platform sources and flags ────────────────────────────────────────────────

S5L8900_SRC := \
	plat-s5l8900/s5l8900.c plat-s5l8900/accel.c plat-s5l8900/aes.c \
	plat-s5l8900/buttons.c plat-s5l8900/chipid.c plat-s5l8900/clock.c \
	plat-s5l8900/dma.c plat-s5l8900/event.c plat-s5l8900/gpio.c \
	plat-s5l8900/i2c.c plat-s5l8900/interrupt.c plat-s5l8900/lcd.c \
	plat-s5l8900/miu.c plat-s5l8900/mmu.c plat-s5l8900/nand.c \
	plat-s5l8900/ftl.c plat-s5l8900/pmu.c plat-s5l8900/power.c \
	plat-s5l8900/sdio.c plat-s5l8900/spi.c plat-s5l8900/timer.c \
	plat-s5l8900/uart.c plat-s5l8900/usbphy.c plat-s5l8900/wdt.c \
	plat-s5l8900/wlan.c
S5L8900_INC  := -Iplat-s5l8900/includes
S5L8900_DEFS := -DARM11 -DCONFIG_S5L8900 -mcpu=arm1176jzf-s

S5L8720_SRC := \
	plat-s5l8720/s5l8720.c plat-s5l8720/accel.c plat-s5l8720/aes.c \
	plat-s5l8720/buttons.c plat-s5l8720/chipid.c plat-s5l8720/clcd.c \
	plat-s5l8720/clock.c plat-s5l8720/dma.c plat-s5l8720/event.c \
	plat-s5l8720/gpio.c plat-s5l8720/i2c.c plat-s5l8720/interrupt.c \
	plat-s5l8720/mipi_dsim.c plat-s5l8720/miu.c plat-s5l8720/mmu.c \
	plat-s5l8720/pmu.c plat-s5l8720/power.c plat-s5l8720/spi.c \
	plat-s5l8720/timer.c plat-s5l8720/uart.c plat-s5l8720/usbphy.c \
	plat-s5l8720/wdt.c
S5L8720_INC  := -Iplat-s5l8720/includes
S5L8720_DEFS := -DARM11 -DCONFIG_S5L8720 -mcpu=arm1176jzf-s

S5L8920_SRC := \
	plat-s5l8920/s5l8920.c plat-s5l8920/usbphy.c plat-s5l8920/aes.c \
	plat-s5l8920/buttons.c plat-s5l8920/chipid.c plat-s5l8920/clock.c \
	plat-s5l8920/event.c plat-s5l8920/gpio.c plat-s5l8920/i2c.c \
	plat-s5l8920/interrupt.c plat-s5l8920/clcd.c plat-s5l8920/mipi_dsim.c \
	plat-s5l8920/miu.c plat-s5l8920/mmu.c plat-s5l8920/power.c \
	plat-s5l8920/spi.c plat-s5l8920/timer.c plat-s5l8920/cdma.c \
	plat-s5l8920/h2fmi.c plat-s5l8920/uart.c
S5L8920_INC  := -Iplat-s5l8920/includes
S5L8920_DEFS := -DARM_A8 -DCONFIG_S5L8920 -DMALLOC_NO_WDT -mcpu=cortex-a8

A4_SRC := \
	plat-a4/a4.c plat-a4/aes.c plat-a4/buttons.c plat-a4/chipid.c \
	plat-a4/clock.c plat-a4/event.c plat-a4/gpio.c plat-a4/i2c.c \
	plat-a4/interrupt.c plat-a4/mipi_dsim.c plat-a4/clcd.c plat-a4/miu.c \
	plat-a4/mmu.c plat-a4/pmu.c plat-a4/power.c plat-a4/spi.c \
	plat-a4/timer.c plat-a4/uart.c plat-a4/cdma.c plat-a4/usbphy.c \
	plat-a4/h2fmi.c plat-a4/sdio.c
A4_INC  := -Iplat-a4/includes
A4_DEFS := -DARM_A8 -DCONFIG_A4 -DMALLOC_NO_WDT -DBIG_FONT -mcpu=cortex-a8

# ── Target definitions ────────────────────────────────────────────────────────
# <T>_SRC      all source files
# <T>_FLAGS    extra compiler flags, defines, includes
# <T>_OUT      output base name (no extension)
# <T>_TEMPLATE img3 template path (empty = .bin output only)

iPhone2G_SRC := \
	$(ARCH_ARM_SRC) $(BASE_SRC) $(S5L8900_SRC) \
	plat-s5l8900/camera.c plat-s5l8900/als-TSL2561.c \
	plat-s5l8900/multitouch-z1.c plat-s5l8900/wm8958.c \
	plat-s5l8900/vibrator-2G.c \
	$(HFS_SRC) $(ACM_SRC) $(USB_SRC) $(NOR_CFI_SRC) $(PMB8876_SRC) $(MENU_SRC)
iPhone2G_FLAGS    := $(S5L8900_DEFS) $(S5L8900_INC) \
	$(ACM_INC) $(USB_INC) $(NOR_CFI_INC) $(PMB8876_INC) $(MENU_INC) \
	-DCONFIG_IPHONE_2G -DMACH_ID=3556 \
	-DOPENIBOOT_VERSION_CONFIG='" for iPhone 2G"'
iPhone2G_OUT      := iphone_2g_openiboot
iPhone2G_TEMPLATE := mk8900image/template.img3

iPhone3G_SRC := \
	$(ARCH_ARM_SRC) $(BASE_SRC) $(S5L8900_SRC) \
	plat-s5l8900/camera.c plat-s5l8900/als-ISL29003.c \
	plat-s5l8900/multitouch-z2.c plat-s5l8900/wm8991.c \
	plat-s5l8900/vibrator-3G.c \
	$(HFS_SRC) $(ACM_SRC) $(USB_SRC) $(NOR_SPI_SRC) $(PMB8878_SRC) $(MENU_SRC)
iPhone3G_FLAGS    := $(S5L8900_DEFS) $(S5L8900_INC) \
	$(ACM_INC) $(USB_INC) $(NOR_SPI_INC) $(PMB8878_INC) $(MENU_INC) \
	-DCONFIG_IPHONE_3G -DMACH_ID=3557 \
	-DOPENIBOOT_VERSION_CONFIG='" for iPhone 3G"'
iPhone3G_OUT      := iphone_3g_openiboot
iPhone3G_TEMPLATE := mk8900image/template-3g.img3

iPodTouch1G_SRC := \
	$(ARCH_ARM_SRC) $(BASE_SRC) $(S5L8900_SRC) \
	plat-s5l8900/piezo.c plat-s5l8900/wm8958.c plat-s5l8900/multitouch-z2.c \
	$(HFS_SRC) $(ACM_SRC) $(USB_SRC) $(NOR_CFI_SRC) $(MENU_SRC)
iPodTouch1G_FLAGS    := $(S5L8900_DEFS) $(S5L8900_INC) \
	$(ACM_INC) $(USB_INC) $(NOR_CFI_INC) $(MENU_INC) \
	-DCONFIG_IPOD_TOUCH_1G -DMACH_ID=3558 \
	-DOPENIBOOT_VERSION_CONFIG='" for iPod Touch 1G"'
iPodTouch1G_OUT      := ipt_1g_openiboot
iPodTouch1G_TEMPLATE := mk8900image/template-ipt1g.img3

iPodTouch2G_SRC := \
	$(ARCH_ARM_SRC) $(S5L8720_BASE_SRC) $(S5L8720_SRC) \
	audiohw-null.c \
	$(HFS_SRC) $(ACM_SRC) $(USB_SRC) $(NOR_SPI_SRC)
iPodTouch2G_FLAGS    := $(S5L8720_DEFS) $(S5L8720_INC) \
	$(ACM_INC) $(USB_INC) $(NOR_SPI_INC) \
	-DCONFIG_IPOD_TOUCH_2G \
	-DOPENIBOOT_VERSION_CONFIG='" for iPod Touch 2G"'
iPodTouch2G_OUT      := ipt_2g_openiboot
iPodTouch2G_TEMPLATE := mk8900image/template-ipt2g.img3

iPhone3GS_SRC := \
	$(ARCH_ARM_SRC) $(BASE_SRC) $(S5L8920_SRC) \
	audiohw-null.c \
	$(HFS_SRC) $(ACM_SRC) $(USB_SRC) $(NOR_SPI_SRC) $(PMB8878_SRC) \
	$(VFL_VFL_SRC) $(VFL_VSVFL_SRC) $(FTL_YAFTL_SRC)
iPhone3GS_FLAGS    := $(S5L8920_DEFS) $(S5L8920_INC) \
	$(ACM_INC) $(USB_INC) $(NOR_SPI_INC) $(PMB8878_INC) \
	$(VFL_VFL_INC) $(VFL_VSVFL_INC) $(FTL_YAFTL_INC) \
	-DCONFIG_IPHONE_3GS -DMACH_ID=3562 \
	-DOPENIBOOT_VERSION_CONFIG='" for iPhone 3GS"'
iPhone3GS_OUT      := iphone_3gs_openiboot
iPhone3GS_TEMPLATE :=

iPhone4_SRC := \
	$(ARCH_ARM_SRC) $(BASE_SRC) $(A4_SRC) \
	audiohw-null.c plat-a4/accel.c \
	$(HFS_SRC) $(ACM_SRC) $(USB_SRC) $(XGOLD618_SRC) \
	$(VFL_VFL_SRC) $(VFL_VSVFL_SRC) $(FTL_YAFTL_SRC)
iPhone4_FLAGS    := $(A4_DEFS) $(A4_INC) \
	$(ACM_INC) $(USB_INC) $(XGOLD618_INC) \
	$(VFL_VFL_INC) $(VFL_VSVFL_INC) $(FTL_YAFTL_INC) \
	-DCONFIG_IPHONE_4 -DMACH_ID=3563 \
	-DOPENIBOOT_VERSION_CONFIG='" for iPhone 4"'
iPhone4_OUT      := iphone_4_openiboot
iPhone4_TEMPLATE :=

iPad1G_SRC := \
	$(ARCH_ARM_SRC) $(BASE_SRC) $(A4_SRC) \
	audiohw-null.c plat-a4/accel.c \
	$(HFS_SRC) $(ACM_SRC) $(USB_SRC) $(NOR_SPI_SRC) \
	$(VFL_VFL_SRC) $(VFL_VSVFL_SRC) $(FTL_YAFTL_SRC)
iPad1G_FLAGS    := $(A4_DEFS) $(A4_INC) \
	$(ACM_INC) $(USB_INC) $(NOR_SPI_INC) \
	$(VFL_VFL_INC) $(VFL_VSVFL_INC) $(FTL_YAFTL_INC) \
	-DCONFIG_IPAD_1G -DMACH_ID=3593 \
	-DOPENIBOOT_VERSION_CONFIG='" for iPad 1G"'
iPad1G_OUT      := ipad_1g_openiboot
iPad1G_TEMPLATE :=

iPodTouch4G_SRC := \
	$(ARCH_ARM_SRC) $(BASE_SRC) $(A4_SRC) \
	audiohw-null.c plat-a4/accel.c \
	$(HFS_SRC) $(ACM_SRC) $(USB_SRC) \
	$(VFL_VFL_SRC) $(VFL_VSVFL_SRC) $(FTL_YAFTL_SRC)
iPodTouch4G_FLAGS    := $(A4_DEFS) $(A4_INC) \
	$(ACM_INC) $(USB_INC) \
	$(VFL_VFL_INC) $(VFL_VSVFL_INC) $(FTL_YAFTL_INC) \
	-DCONFIG_IPOD_TOUCH_4G -DMACH_ID=3564 \
	-DOPENIBOOT_VERSION_CONFIG='" for iPod Touch 4G"'
iPodTouch4G_OUT      := ipt_4g_openiboot
iPodTouch4G_TEMPLATE :=

aTV2G_SRC := \
	$(ARCH_ARM_SRC) $(BASE_SRC) $(A4_SRC) \
	audiohw-null.c plat-a4/mcu.c \
	$(HFS_SRC) $(ACM_SRC) $(USB_SRC) \
	$(VFL_VFL_SRC) $(VFL_VSVFL_SRC) $(FTL_YAFTL_SRC)
aTV2G_FLAGS    := $(A4_DEFS) $(A4_INC) \
	$(ACM_INC) $(USB_INC) \
	$(VFL_VFL_INC) $(VFL_VSVFL_INC) $(FTL_YAFTL_INC) \
	-DCONFIG_ATV_2G -DMACH_ID=3594 \
	-DOPENIBOOT_VERSION_CONFIG='" for aTV 2G"'
aTV2G_OUT      := atv_2g_openiboot
aTV2G_TEMPLATE :=

# ── Debug variants (adds -DDEBUG -g, output name gets _debug suffix) ──────────

iPhone2GD_SRC      := $(iPhone2G_SRC)
iPhone2GD_FLAGS    := $(iPhone2G_FLAGS) -DDEBUG -g
iPhone2GD_OUT      := iphone_2g_openiboot_debug
iPhone2GD_TEMPLATE := $(iPhone2G_TEMPLATE)

iPhone3GD_SRC      := $(iPhone3G_SRC)
iPhone3GD_FLAGS    := $(iPhone3G_FLAGS) -DDEBUG -g
iPhone3GD_OUT      := iphone_3g_openiboot_debug
iPhone3GD_TEMPLATE := $(iPhone3G_TEMPLATE)

iPodTouch1GD_SRC      := $(iPodTouch1G_SRC)
iPodTouch1GD_FLAGS    := $(iPodTouch1G_FLAGS) -DDEBUG -g
iPodTouch1GD_OUT      := ipt_1g_openiboot_debug
iPodTouch1GD_TEMPLATE := $(iPodTouch1G_TEMPLATE)

iPodTouch2GD_SRC      := $(iPodTouch2G_SRC)
iPodTouch2GD_FLAGS    := $(iPodTouch2G_FLAGS) -DDEBUG -g
iPodTouch2GD_OUT      := ipt_2g_openiboot_debug
iPodTouch2GD_TEMPLATE := $(iPodTouch2G_TEMPLATE)

iPhone3GSD_SRC      := $(iPhone3GS_SRC)
iPhone3GSD_FLAGS    := $(iPhone3GS_FLAGS) -DDEBUG -g
iPhone3GSD_OUT      := iphone_3gs_openiboot_debug
iPhone3GSD_TEMPLATE := $(iPhone3GS_TEMPLATE)

iPhone4D_SRC      := $(iPhone4_SRC)
iPhone4D_FLAGS    := $(iPhone4_FLAGS) -DDEBUG -g
iPhone4D_OUT      := iphone_4_openiboot_debug
iPhone4D_TEMPLATE := $(iPhone4_TEMPLATE)

iPad1GD_SRC      := $(iPad1G_SRC)
iPad1GD_FLAGS    := $(iPad1G_FLAGS) -DDEBUG -g
iPad1GD_OUT      := ipad_1g_openiboot_debug
iPad1GD_TEMPLATE := $(iPad1G_TEMPLATE)

iPodTouch4GD_SRC      := $(iPodTouch4G_SRC)
iPodTouch4GD_FLAGS    := $(iPodTouch4G_FLAGS) -DDEBUG -g
iPodTouch4GD_OUT      := ipt_4g_openiboot_debug
iPodTouch4GD_TEMPLATE := $(iPodTouch4G_TEMPLATE)

aTV2GD_SRC      := $(aTV2G_SRC)
aTV2GD_FLAGS    := $(aTV2G_FLAGS) -DDEBUG -g
aTV2GD_OUT      := atv_2g_openiboot_debug
aTV2GD_TEMPLATE := $(aTV2G_TEMPLATE)

# ── Installer variants (s5l8900 only — swaps menu module for installer) ───────

iPhone2G-Installer_SRC := \
	$(filter-out $(MENU_SRC),$(iPhone2G_SRC)) $(INSTALLER_SRC)
iPhone2G-Installer_FLAGS := \
	$(filter-out $(MENU_INC),$(iPhone2G_FLAGS)) $(INSTALLER_INC)
iPhone2G-Installer_OUT      := iphone_2g_installer
iPhone2G-Installer_TEMPLATE := $(iPhone2G_TEMPLATE)

iPhone3G-Installer_SRC := \
	$(filter-out $(MENU_SRC),$(iPhone3G_SRC)) $(INSTALLER_SRC)
iPhone3G-Installer_FLAGS := \
	$(filter-out $(MENU_INC),$(iPhone3G_FLAGS)) $(INSTALLER_INC)
iPhone3G-Installer_OUT      := iphone_3g_installer
iPhone3G-Installer_TEMPLATE := $(iPhone3G_TEMPLATE)

iPodTouch1G-Installer_SRC := \
	$(filter-out $(MENU_SRC),$(iPodTouch1G_SRC)) $(INSTALLER_SRC)
iPodTouch1G-Installer_FLAGS := \
	$(filter-out $(MENU_INC),$(iPodTouch1G_FLAGS)) $(INSTALLER_INC)
iPodTouch1G-Installer_OUT      := ipt_1g_installer
iPodTouch1G-Installer_TEMPLATE := $(iPodTouch1G_TEMPLATE)

TARGETS := \
	iPhone2G iPhone3G iPodTouch1G iPodTouch2G \
	iPhone3GS iPhone4 iPad1G iPodTouch4G aTV2G \
	iPhone2GD iPhone3GD iPodTouch1GD iPodTouch2GD \
	iPhone3GSD iPhone4D iPad1GD iPodTouch4GD aTV2GD \
	iPhone2G-Installer iPhone3G-Installer iPodTouch1G-Installer

# ── Build rule template ───────────────────────────────────────────────────────

# src_to_obj T, SRCS — convert source paths to build/<T>/path.o
src_to_obj = $(patsubst %.sx,$(BUILD)/$(1)/%.o,$(patsubst %.c,$(BUILD)/$(1)/%.o,$(2)))

# order_objs T, OBJS — put entry.o first, init.o second, sentinel.o last
order_objs = \
	$(filter $(BUILD)/$(1)/arch-arm/entry.o,$(2)) \
	$(filter $(BUILD)/$(1)/init.o,$(2)) \
	$(filter-out \
		$(BUILD)/$(1)/arch-arm/entry.o \
		$(BUILD)/$(1)/init.o \
		$(BUILD)/$(1)/sentinel.o,$(2)) \
	$(filter $(BUILD)/$(1)/sentinel.o,$(2))

define TARGET_RULES

# All sources including the mandatory bookend files
$(1)_ALLSRC  := $$($(1)_SRC) init.c sentinel.c
$(1)_ALLOBJS := $$(call src_to_obj,$(1),$$($(1)_ALLSRC))
$(1)_OBJS    := $$(call order_objs,$(1),$$($(1)_ALLOBJS))
$(1)_ELF     := $(BUILD)/$(1)/$$($(1)_OUT)

# Compile C sources
$(BUILD)/$(1)/%.o: %.c
	@mkdir -p $$(@D)
	$(CC) $(CFLAGS) $(INCLUDES) $$($(1)_FLAGS) -MMD -MP -c -o $$@ $$<

# Compile preprocessed assembly sources
$(BUILD)/$(1)/%.o: %.sx
	@mkdir -p $$(@D)
	$(CC) $(ASFLAGS) $(INCLUDES) $$($(1)_FLAGS) -c -o $$@ $$<

# Link ELF
$$($(1)_ELF): $$($(1)_OBJS)
	$(CC) $(LDFLAGS) $$($(1)_FLAGS) -o $$@ $$^ -lgcc

# Package with mk8900image
ifneq ($$($(1)_TEMPLATE),)
# Has an img3 template — produce both .bin and .img3
$$($(1)_OUT).bin: $$($(1)_ELF) $(MK8900IMG)
	$(MK8900IMG) $$< $$@

$$($(1)_OUT).img3: $$($(1)_ELF) $$($(1)_TEMPLATE) $(MK8900IMG)
	$(MK8900IMG) $$< $$@ $$($(1)_TEMPLATE)

$(1): $$($(1)_OUT).img3
else
# No template — produce .bin only
$$($(1)_OUT).bin: $$($(1)_ELF) $(MK8900IMG)
	$(MK8900IMG) $$< $$@

$(1): $$($(1)_OUT).bin
endif

.PHONY: $(1)

# If this target includes installer.c, its object file needs the generated headers
ifneq ($$(filter $(INSTALLER_SRC),$$($(1)_SRC)),)
$(BUILD)/$(1)/installer/installer.o: $(INSTALLER_IMGS)
endif

-include $$(patsubst %.o,%.d,$$($(1)_ALLOBJS))

endef

# ── Installer image headers ───────────────────────────────────────────────────

installer/images/%PNG.h: installer/images/%.png $(BIN2C)
	$(BIN2C) data$(*F)PNG < $< > $@

# ── Generate rules for all targets ───────────────────────────────────────────

$(foreach T,$(TARGETS),$(eval $(call TARGET_RULES,$(T))))

# ── Utility targets ───────────────────────────────────────────────────────────

.DEFAULT_GOAL := help

help:
	@echo "Usage: make <target>"
	@echo ""
	@echo "Release builds:"
	@echo "  s5l8900:  iPhone2G    iPhone3G    iPodTouch1G"
	@echo "  s5l8720:  iPodTouch2G"
	@echo "  s5l8920:  iPhone3GS"
	@echo "  a4:       iPhone4     iPad1G      iPodTouch4G  aTV2G"
	@echo ""
	@echo "Debug builds (append D, e.g. iPhone2GD):"
	@echo "  iPhone2GD  iPhone3GD  iPodTouch1GD  iPodTouch2GD"
	@echo "  iPhone3GSD iPhone4D   iPad1GD       iPodTouch4GD  aTV2GD"
	@echo ""
	@echo "Installer builds (s5l8900 only):"
	@echo "  iPhone2G-Installer  iPhone3G-Installer  iPodTouch1G-Installer"
	@echo ""
	@echo "Other:"
	@echo "  docs      Build Doxygen documentation"
	@echo ""
	@echo "Options: CROSS=<prefix>  (default: arm-none-eabi-)"

docs:
	doxygen Doxyfile

clean:
	rm -rf $(BUILD)
	rm -f $(foreach T,$(TARGETS),$($(T)_OUT).bin $($(T)_OUT).img3)
	rm -f $(INSTALLER_IMGS)

.PHONY: help docs clean
