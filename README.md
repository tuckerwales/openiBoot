# openiBoot

An open-source bootloader for Apple's ARM-based devices (iPhone, iPod touch, iPad, Apple TV). Originally developed by the [iDroid Project](https://www.idroidproject.org/) to boot alternative operating systems (Android, Linux) on early Apple hardware.

## Supported Devices

| Device | SoC | CPU | Status |
|---|---|---|---|
| iPhone 2G | S5L8900 | ARM1176JZF-S | Stable |
| iPhone 3G | S5L8900 | ARM1176JZF-S | Stable |
| iPod Touch 1G | S5L8900 | ARM1176JZF-S | Stable |
| iPod Touch 2G | S5L8720 | ARM1176JZF-S | Stable |
| iPhone 3GS | S5L8920 | Cortex-A8 | Experimental |
| iPhone 4 | A4 | Cortex-A8 | Experimental |
| iPad 1G | A4 | Cortex-A8 | Experimental |
| iPod Touch 4G | A4 | Cortex-A8 | Experimental |
| Apple TV 2G | A4 | Cortex-A8 | Experimental |

## Building

### Prerequisites

- **ARM cross-compiler:** `arm-none-eabi-gcc` must be on your PATH.
  - macOS: Install [Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) or `brew install --cask gcc-arm-embedded`
  - Linux: `sudo apt install gcc-arm-none-eabi` (Debian/Ubuntu) or download from Arm
- **GNU Make**

### Build a target

```sh
make iPhone3G
```

Available targets:

| SoC | Release | Debug | Installer |
|---|---|---|---|
| S5L8900 | `iPhone2G` `iPhone3G` `iPodTouch1G` | `iPhone2GD` `iPhone3GD` `iPodTouch1GD` | `iPhone2G-Installer` `iPhone3G-Installer` `iPodTouch1G-Installer` |
| S5L8720 | `iPodTouch2G` | `iPodTouch2GD` | |
| S5L8920 | `iPhone3GS` | `iPhone3GSD` | |
| A4 | `iPhone4` `iPad1G` `iPodTouch4G` `aTV2G` | `iPhone4D` `iPad1GD` `iPodTouch4GD` `aTV2GD` | |

Debug builds add `-DDEBUG -g`, which enables `DebugPrintf` output and debug symbols.

### Build output

- **`.img3`** — For loading via recovery/iBoot mode (S5L8900, S5L8720 devices)
- **`.bin`** — For loading via DFU mode (S5L8920, A4 devices)

### Cross-compiler override

```sh
make iPhone3G CROSS=arm-none-eabi-
```

### Other targets

```sh
make clean    # Remove all build artifacts
make docs     # Generate Doxygen documentation
```

## Installing

See [INSTALLING.md](INSTALLING.md) for detailed instructions on flashing openiBoot to a device, including:

- **Bootlace** (Cydia app, easiest for S5L8900 devices)
- **Manual install** via recovery mode with `loadibec` + `oibc`
- **DFU mode** loading for S5L8920/A4 devices
- **Restoring** the original bootloader

## Menu Configuration

openiBoot includes a GRUB-style boot menu. On startup it looks for `/boot/menu.lst` on the data partition. If the file is not found, it drops to the interactive console.

Example `/boot/menu.lst`:

```
title iOS
auto

title Android
kernel "(hd0,1)/idroid/zImage" "console=tty root=/dev/ram0 init=/init rw"
initrd "(hd0,1)/idroid/android.img.gz"

title iX
kernel "(hd0,1)/iX/zImage" "console=tty root=/dev/ram0 init=/init rw"
initrd "(hd0,1)/iX/initrd.img.gz"
```

**Menu controls (on device):**

| Button | Action |
|---|---|
| Home | Select / boot highlighted entry |
| Hold switch (or Volume Down) | Next entry |
| Volume Up | Previous entry |

"Console" is always listed as the last menu entry and opens the interactive USB console.

## Interactive Console

openiBoot provides an interactive console accessible over USB using the `oibc` (openiBoot Console) tool. The console supports commands for device inspection, NOR operations, memory access, booting, and more. Type `help` in the console for a list of available commands.

## Project Structure

```
arch-arm/          ARM architecture support (exception vectors, context switch, asm helpers)
plat-s5l8900/      S5L8900 platform drivers (iPhone 2G/3G, iPod Touch 1G)
plat-s5l8720/      S5L8720 platform drivers (iPod Touch 2G)
plat-s5l8920/      S5L8920 platform drivers (iPhone 3GS)
plat-a4/           A4 platform drivers (iPhone 4, iPad 1G, iPod Touch 4G, Apple TV 2G)
includes/          Shared headers
acm/               USB Abstract Control Model (virtual serial port)
usb-synopsys/      Synopsys USB OTG driver
nor-cfi/           CFI NOR flash driver
nor-spi/           SPI NOR flash driver
vfl-vfl/           Virtual Flash Layer
vfl-vsvfl/         VSVFL implementation
ftl-yaftl/         YAFTL Flash Translation Layer
hfs/               HFS+ filesystem support
menu/              Boot menu UI
installer/         On-device installer
radio-pmb8876/     PMB8876 baseband radio driver
radio-pmb8878/     PMB8878 baseband radio driver
radio-xgold618/    XGold 618 baseband radio driver
mk8900image/       Tool to package binaries into img3 format
images/             Boot images and bin2c converter
```

## Architecture Notes

- All C code compiles as **Thumb** (`-mthumb`) with ARM interworking (`-mthumb-interwork`).
- Exception handlers and coprocessor manipulation are in **ARM mode** (`.code 32`).
- The cooperative task scheduler (`tasks.c`) uses manual context switching via `SwapTask` in assembly.
- Critical sections use IRQ/FIQ disable via direct CPSR manipulation.
- The VIC (PL192) interrupt controller dispatches through a handler table in the `.data` section.

## License

GNU General Public License v3.0 or later. See [LICENSE.txt](LICENSE.txt).

Copyright (C) 2008-2011 iDroid Project. Originally created by David Wang (planetbeing).

## Links

- [iDroid Project](https://www.idroidproject.org/)
- [Original repository](https://github.com/iDroid-Project/openiBoot)
