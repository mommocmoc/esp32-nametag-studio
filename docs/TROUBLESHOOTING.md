# When it doesn't work

Start with `./tools/doctor.sh`. It checks the toolchain, the project files, and
the board, and prints what's wrong. Pasting its output to a coding agent is
usually faster than reading this page.

---

## Setup

**`arduino-cli: command not found` after running setup.sh**

It installed to `~/.local/bin`, which isn't on your `PATH`. Add it:

```bash
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.zshrc && source ~/.zshrc
```

**The ESP32 core download is enormous / stalls**

It is — about 1GB of compiler toolchain, and it's a one-time cost. If it stalls,
re-run `./tools/setup.sh`; it resumes rather than starting over.

**`Error: Platform 'esp32:esp32' not found`**

The board index didn't register. Run it by hand:

```bash
arduino-cli config add board_manager.additional_urls \
  https://espressif.github.io/arduino-esp32/package_esp32_index.json
arduino-cli core update-index
arduino-cli core install esp32:esp32
```

---

## Building

**`No such file or directory: Arduino_GFX_Library.h`**

Libraries aren't installed. `./tools/setup.sh` handles it, or:

```bash
arduino-cli lib install "GFX Library for Arduino" PNGdec JPEGDEC
```

**`text section exceeds available space in board`**

Wrong partition scheme. The build needs `PartitionScheme=huge_app`, which
`tools/build.sh` sets automatically — you'll only hit this compiling from the
Arduino IDE. Set **Tools → Partition Scheme → Huge APP**.

**`warning: "INTELSHORT" redefined`** *(and MOTOLONG, INTELLONG, MOTOSHORT)*

Expected. `PNGdec` and `JPEGDEC` both define these macros. Upstream noise, not
your bug, and the build succeeds.

**`fatal error: name_bitmap.h: No such file`**

`USE_NAME_BITMAP` is `true` but you haven't generated the bitmap. Either:

```bash
python3 tools/make_name_bitmap.py "Your Name"
```

or set `USE_NAME_BITMAP` back to `false`.

**`#error Unknown NAMETAG_THEME`**

Typo in the theme name. It must exactly match one defined in `theme.h` —
`THEME_KITSCH_POP`, `THEME_MIDNIGHT`, `THEME_MONO_INK`, `THEME_CANDY`,
`THEME_TERMINAL`.

---

## Uploading

**No board detected**

In order of likelihood:

1. **The cable.** Charge-only USB-C cables are physically identical to data
   cables and this is the single most common cause. Try a different one.
2. Plug into the board's own USB-C port, not a port on an add-on module.
3. Check what the system sees: `arduino-cli board list`
4. Bypass detection if you know the port: `./tools/flash.sh /dev/cu.usbmodem101`

**Upload starts, then fails or hangs**

Put the board into bootloader mode manually:

> Hold **BOOT** → tap **RESET** → release **BOOT** → re-run `./tools/flash.sh`

This is normal ESP32-S3 behavior, not a defect. Some boards need it every time;
most need it only after a sketch that crashes on boot.

**`Permission denied: /dev/ttyUSB0`** *(Linux)*

```bash
sudo usermod -a -G dialout $USER      # then log out and back in
```

**Upload succeeds but the screen stays black**

Watch the log: `./tools/monitor.sh`

- `[LCD] init FAILED` → wrong board or wrong pins in `config.h` §6
- Nothing at all → USB CDC isn't on, or the cable is charge-only
- `[LCD] ready (320x240)` but a black screen → check `NAMETAG_BRIGHTNESS` isn't 0

---

## On the badge

**Text is cut off**

Fixed-width font, no wrapping, silent clipping. Budgets in
[CUSTOMIZE.md](CUSTOMIZE.md#text) — or widen the pill in `layout.h`.

**My name renders as blank or garbage**

`NAMETAG_NAME` is ASCII-only. Korean, Japanese, Chinese, Cyrillic, and emoji
need the bitmap path:

```bash
python3 tools/make_name_bitmap.py "김민준" --preview
```

`--preview` shows you the glyphs as ASCII art. If that's empty or wrong, your
font lacks those characters — pass a different one with `--font`.

**The bitmap name is clipped**

It's wider than the ~180px name area. Regenerate smaller — the script tells you
what `--size` to use.

**`SD: NO-CARD` in the footer**

The card isn't mounting. Format it **FAT32** (not exFAT — that silently fails).
Cards over 32GB usually ship exFAT and need reformatting. Reseat it, and confirm
in `./tools/monitor.sh` that `[SD] mounted` appears.

**An image doesn't appear**

`./tools/monitor.sh` prints every load attempt and its outcome.

- `[IMG] FAILED` → the path in `config.h` doesn't match the filename on the
  card. Case matters. Files go in the root: `/avatar.png`, not `/images/avatar.png`.
- Loads but looks wrong → images aren't scaled on the device. Run it through
  `tools/prepare_image.py`.
- PNG shows a black box instead of transparency → it was saved without an alpha
  channel. `prepare_image.py --slot avatar` adds one.

**Animation leaves smears or trails**

Something's erasing to the wrong color. Elements inside a card must clear to
`C_CARD`, elements on the page to `C_BG`. If you moved an element in `layout.h`,
check that its clear-rectangle uses the same macros.

---

## Still stuck

Open the folder in a coding agent and give it the evidence:

> Here's my doctor output and the build error. What's wrong?
>
> ```
> [paste ./tools/doctor.sh output]
> [paste the error from ./tools/build.sh]
> ```

Real output beats a description of the output every time.
