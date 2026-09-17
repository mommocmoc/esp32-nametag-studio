# Hardware reference

## The board

**Waveshare ESP32-S3-Touch-LCD-2** — around $25, and the only thing you need to
buy. Everything the badge uses is already on it.

| | |
|---|---|
| MCU | ESP32-S3R8 · dual-core Xtensa LX7 @ 240MHz |
| PSRAM | 8MB OPI |
| Flash | 16MB |
| Display | 2.0″ IPS, 240×320 native, ST7789 driver |
| Touch | CST816D over I²C *(present; this project doesn't use it yet)* |
| Storage | microSD slot |
| Sensors | QMI8658 6-axis IMU, battery charging circuit |
| Power | USB-C, plus a JST connector for a LiPo |

The badge runs the panel rotated to landscape, so you work in 320×240.

## Pins

The only ones this project touches. All defined in `nametag/config.h` §6.

| Function | GPIO |
|---|---|
| LCD SCLK | 39 |
| LCD MOSI | 38 |
| LCD MISO | 40 |
| LCD DC | 42 |
| LCD CS | 45 |
| LCD RST | *(none — software reset)* |
| LCD Backlight | 1 |
| microSD CS | 41 |

The SD card shares the SPI bus with the display, which is why `setup()`
initializes SPI explicitly before mounting.

Unused by this project but available if you extend it: touch on GPIO 47/48,
and a 24-pin FPC camera connector.

## Board settings

`tools/common.sh` encodes these as one FQBN string:

```
esp32:esp32:esp32s3:CDCOnBoot=cdc,FlashSize=16M,PSRAM=opi,PartitionScheme=huge_app
```

Building from the Arduino IDE instead? Match these under **Tools**:

| Setting | Value |
|---|---|
| Board | ESP32S3 Dev Module |
| USB CDC On Boot | **Enabled** — without it you get no serial output |
| Flash Size | 16MB (128Mb) |
| Flash Mode | QIO 80MHz |
| PSRAM | OPI PSRAM |
| Partition Scheme | **Huge APP (3MB No OTA/1MB SPIFFS)** |
| CPU Frequency | 240MHz (WiFi) |
| Upload Speed | 921600 |

Two of those are not optional. **USB CDC On Boot** is how `Serial` reaches your
terminal over the single USB-C port. **Huge APP** is required because the
graphics library plus both image decoders overflow the default partition — you
get a cryptic "text section exceeds available space" if you skip it.

Current build: ~478KB, about 15% of the 3MB app partition. Lots of room.

## Power and wearing it

USB-C powers it from any laptop or power bank. For an untethered badge, connect
a 3.7V LiPo to the JST-PH connector — the board charges it over USB and switches
sources automatically.

Rough runtime at default 85% brightness: a 500mAh cell lasts most of a
conference day. Drop `NAMETAG_BRIGHTNESS` to 50 in `config.h` and it stretches
considerably — the backlight dominates the power budget, not the CPU.

The mounting holes take a standard lanyard clip. A 3D-printed sled makes it look
intentional rather than like a dev board on a string.

## microSD cards

Format **FAT32**. exFAT will not mount. Cards up to 32GB are safest; larger ones
often ship exFAT-formatted and need reformatting.

Files go in the card's root directory, named to match the paths in `config.h`
(`/avatar.png`, `/sticker.png`, and so on).

PNG with an alpha channel is fully supported and is what you want for cut-outs.
JPEG decodes faster and suits full-screen backgrounds.

Images are drawn at their native size — there is no scaling on the device. Use
`tools/prepare_image.py` to size them correctly, or the badge will paint a
4000px photo across everything.

## Porting to another board

Any ESP32 with an ST7789 panel can run this. Change the eight pin defines in
`config.h` §6, set `LCD_PANEL_W` / `LCD_PANEL_H` to your panel's native
resolution, and update `FQBN` in `tools/common.sh`.

If your panel isn't 320×240 after rotation, also update `SCREEN_W` / `SCREEN_H`
in `layout.h`. The layout is hand-tuned for 320×240 — on a different size,
expect to nudge the card coordinates. Everything you'd need to touch is in that
one file.

A different driver chip (ILI9341, GC9A01, …) means swapping the `Arduino_ST7789`
constructor in `nametag.ino` for the matching class. Arduino_GFX supports most
of them and the rest of the code doesn't care.
