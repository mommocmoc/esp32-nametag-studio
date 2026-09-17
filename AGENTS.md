# AGENTS.md

Instructions for a coding agent (Claude Code, Codex, Antigravity, Cursor, …)
operating this repository. Humans want `README.md` instead.

## What this project is

Firmware for a programmable name badge. It renders a two-card layout on a
320×240 ST7789 LCD driven by an ESP32-S3, with optional microSD artwork and
three small animations. One sketch, no build system beyond `arduino-cli`.

## The one rule

**User-facing changes go in `nametag/config.h`. Nothing else.**

When someone asks for a different name, role, color scheme, or animation, edit
`config.h` and stop. `nametag.ino` is the rendering engine — changing it to
accomplish a config-level request is the most common way to break this project.

Escalate beyond `config.h` only when the request genuinely needs it:

| Request | File to edit |
|---|---|
| Name, role, org, status, tags, footer | `nametag/config.h` |
| Switch theme, brightness, rotation | `nametag/config.h` |
| Turn animations on/off | `nametag/config.h` |
| Enable a microSD image slot | `nametag/config.h` |
| **New** color palette | `nametag/theme.h` — add a block, register it in the `#if` chain |
| Move/resize a card, pill, or sparkle | `nametag/layout.h` |
| New UI element, new animation | `nametag/nametag.ino` |
| Non-Latin name | run `tools/make_name_bitmap.py`, set `USE_NAME_BITMAP true` |

## Commands

```bash
./tools/setup.sh      # install arduino-cli, ESP32 core, libraries (idempotent)
./tools/doctor.sh     # diagnose the environment; read this before guessing
./tools/build.sh      # compile only — no board required
./tools/flash.sh      # compile + upload (auto-detects the port)
./tools/monitor.sh    # serial log at 115200
./tools/verify.sh     # compile every theme + both name modes (~1 min)
```

Board FQBN, if you need to invoke `arduino-cli` directly:

```
esp32:esp32:esp32s3:CDCOnBoot=cdc,FlashSize=16M,PSRAM=opi,PartitionScheme=huge_app
```

## Working rules

1. **Always `./tools/build.sh` after editing.** A compile takes under a minute
   and is the only proof the change is valid. Never report success without it.
2. **Never flash unless the user asked you to.** `build.sh` is the safe default;
   `flash.sh` writes to physical hardware.
3. **Run `./tools/verify.sh` after touching `theme.h`, `layout.h`, or
   `nametag.ino`.** A change can compile under the active theme and fail under
   another.
4. **Don't invent colors as raw hex.** Run `python3 tools/rgb565.py "#ff7e9e"`
   and use what it prints. RGB565 is not RGB888.
5. **Check text length.** The display uses a fixed 6px-wide font. A pill clips
   silently — no error, just a cut-off word. Budget roughly:
   `NAMETAG_ROLE` ≤ 22 chars · `NAMETAG_ORG` ≤ 13 · `NAMETAG_STATUS` ≤ 14 ·
   `NAMETAG_TAG_1` ≤ 14 · `TAG_2` ≤ 7 · `TAG_3` ≤ 16 · `NAMETAG_NAME` ≤ 13
   (size-2 font). Over budget? Shorten the text, or widen the pill in `layout.h`.
6. **`NAMETAG_NAME` is ASCII-only.** Any other script needs the bitmap path.
   Don't paste Korean/Japanese/Chinese into that macro and expect it to render —
   it will compile and display nothing.
7. **Preserve the comments in `config.h`.** They are the product; a user with no
   C++ background reads them to understand what they're changing.

## Non-Latin names

```bash
python3 tools/make_name_bitmap.py "김민준" --size 28 --preview
```

`--preview` prints the glyphs as ASCII art, so you can confirm the render
worked without hardware. Verify that before flashing: a missing font silently
produces an empty or garbled bitmap. The script writes `nametag/name_bitmap.h`;
then set `USE_NAME_BITMAP true` in `config.h`.

Keep the result within 180×40 px. The script warns and suggests a `--size` if
it overflows.

## Gotchas

- `PNGdec` and `JPEGDEC` both define `INTELSHORT`/`MOTOLONG`. The redefinition
  warnings during every build are expected upstream noise. Not your bug.
- `PartitionScheme=huge_app` is mandatory. The default scheme cannot hold the
  graphics library plus both image decoders.
- No microSD card is a supported state, not an error. The badge falls back to
  the drawn UI and prints `SD: NO-CARD` in its footer.
- Animations repaint a small rectangle in the card's background color. If you
  move an animated element in `layout.h`, its clear-rectangle follows
  automatically — that's why those coordinates are macros, not literals.
- The board sometimes needs a manual bootloader entry: hold **BOOT**, tap
  **RESET**, release **BOOT**, then re-run `flash.sh`.

## Definition of done

- [ ] `./tools/build.sh` passes
- [ ] `./tools/verify.sh` passes, if `theme.h` / `layout.h` / `nametag.ino` changed
- [ ] Text fits the character budgets above
- [ ] Told the user what to run next (`./tools/flash.sh`), rather than flashing
