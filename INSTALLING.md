# Installing openiBoot on a Device

This document covers how to flash openiBoot onto a supported device. For build instructions, see [README.md](README.md).

> **WARNING (A4 devices):** DO NOT attempt to permanently flash openiBoot to the NOR on iPhone 4, iPad 1G, iPod Touch 4G, or Apple TV 2G. Doing so may brick your device. These devices are only supported for temporary (RAM) loading at this time.

---

## Supported Devices

| Device | Chip | Permanent NOR install | Temporary (RAM) load |
|---|---|---|---|
| iPhone 2G | s5l8900 | Yes | Yes |
| iPhone 3G | s5l8900 | Yes | Yes |
| iPod Touch 1G | s5l8900 | Yes | Yes |
| iPod Touch 2G | s5l8720 | Yes | Yes |
| iPhone 3GS | s5l8920 | With caution | Yes |
| iPhone 4 | A4 | **No** | Yes |
| iPad 1G | A4 | **No** | Yes |
| iPod Touch 4G | A4 | **No** | Yes |
| Apple TV 2G | A4 | **No** | Yes |

---

## Prerequisites

### 1. Jailbreak your device

openiBoot requires a jailbreak that permits unsigned NOR images. The following jailbreaks are compatible:

- **redsn0w**
- **PwnageTool**
- **Blackra1n**

The following jailbreaks are **not** compatible:

- Spirit
- JailbreakMe (any version)

### 2. iOS version

openiBoot is tested against iOS 3.1.2 through 4.1 on s5l8900 devices. iOS 4.2.1 is not yet fully supported.

### 3. Build or obtain the openiBoot binary

You will need either:
- The `.img3` file (for loading via recovery/iBoot mode — s5l8900/s5l8720 devices)
- The `.bin` file (for loading via DFU mode — s5l8920/A4 devices)

See [README.md](README.md) for how to build these from source, or download a pre-built release.

### 4. Companion tools

The following tools are required for manual installation and are separate from this repository:

- **`loadibec`** — sends an img3 to the device in recovery mode
- **`oibc`** — openiBoot console, used to interact with openiBoot over USB once loaded

These tools are available from the [iDroid Project](https://www.idroidproject.org/) and the [`syringe`](https://github.com/iDroid-Project/syringe) repository.

---

## Method 1: Bootlace (easiest — s5l8900 only)

Bootlace is a Cydia application that installs openiBoot directly from your device. No computer required.

**Supports:** iPhone 2G, iPhone 3G, iPod Touch 1G

1. Open **Cydia** on your jailbroken device.
2. Search for **Bootlace** (hosted by BigBoss) and install it.
3. Open **Bootlace**. It will check your iOS version and jailbreak compatibility.
4. Tap the **OpeniBoot** tab (boot icon, second from right).
5. Tap **Install OpeniBoot**. Bootlace will patch the kernel and flash openiBoot to NOR.
6. The device will reboot into openiBoot on completion.

> A backup of your original NOR (`norbackup.dump`) is saved to the device. Keep this file — it is the only way to restore your original bootloader if something goes wrong.

---

## Method 2: Manual install via recovery mode (s5l8900/s5l8720)

**Supports:** iPhone 2G, iPhone 3G, iPod Touch 1G, iPod Touch 2G

### Step 1 — Enter recovery mode

Hold **Home + Power** until the device powers off, then continue holding until the iTunes "connect to iTunes" screen appears.

### Step 2 — Load openiBoot into RAM

From your computer, run:

```sh
loadibec openiboot.img3
```

openiBoot will boot from RAM. The device screen will show the openiBoot interface.

### Step 3 — Connect with oibc

```sh
oibc
```

This opens an interactive console connected to your device over USB.

### Step 4 — Flash to NOR (permanent install)

Inside the `oibc` console, type:

```
install
```

openiBoot will back up your existing NOR to `norbackup.dump` and then write itself to NOR. This takes approximately 30–60 seconds. When complete, the device will display:

```
Openiboot installation complete.
```

Your device will now boot openiBoot on every power-on.

---

## Method 3: DFU mode load (s5l8920 / A4)

**Supports:** iPhone 3GS, iPhone 4, iPad 1G, iPod Touch 4G, Apple TV 2G

> These devices can only load openiBoot temporarily into RAM via DFU mode. Do not attempt a permanent NOR install on A4 devices.

### Step 1 — Enter DFU mode

1. Power off the device completely.
2. Hold **Power** for 3 seconds.
3. While still holding Power, also hold **Home** for 10 seconds.
4. Release Power but continue holding **Home** for a further 5 seconds.
5. The screen should remain black. iTunes (or `idevicerestore`) should detect the device in DFU mode.

### Step 2 — Send the openiBoot binary

Use a DFU-mode upload tool (e.g. `redsn0w`, `irecovery`, or a platform-specific exploit tool) to send the compiled `.bin` file:

```sh
irecovery -f openiboot.bin
```

openiBoot will load and run from RAM.

---

## Restoring the original bootloader

If you need to restore your original NOR after a permanent install:

1. Load openiBoot via recovery mode using `loadibec`.
2. In the `oibc` console, run:

   ```
   norboot
   ```

   This will restore the NOR backup (`norbackup.dump`) that was saved during installation.

Alternatively, use iTunes to restore the device to factory firmware via DFU mode.

---

## Sources

- [Running/Installing — iDroid-Project/openiBoot Wiki](https://github.com/iDroid-Project/openiBoot/wiki/Running-Installing)
- [Installing OpeniBoot — iDroid Project](https://www.idroidproject.org/wiki/Installing_OpeniBoot/)
- [Guide: Installing OpeniBoot and Android on your iDevice — GBAtemp](https://gbatemp.net/threads/guide-installing-openiboot-and-android-on-your-idevice.253635/)
- [Guide: iDroid with Bootlace — GBAtemp](https://gbatemp.net/threads/guide-idroid-with-bootlace.263038/)
- [OpeniBoot — Wikipedia](https://en.wikipedia.org/wiki/OpeniBoot)
