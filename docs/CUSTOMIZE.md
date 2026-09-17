# Customizing

Three files, three jobs. Knowing which one to open is most of the skill.

| File | Holds |
|---|---|
| `nametag/config.h` | Your text, theme choice, toggles, image slots |
| `nametag/theme.h` | Color palettes |
| `nametag/layout.h` | Every coordinate on screen |

`nametag.ino` is the engine. You can go a long way without opening it.

---

## Text

All in `config.h`, section 1. Change what's between the quotes.

The display uses a fixed-width font — 6px per character at normal size, 12px at
the double size the name uses. Pills don't wrap and don't warn; they just clip.

| Field | Fits about |
|---|---|
| `NAMETAG_NAME` | 13 characters |
| `NAMETAG_ROLE` | 22 |
| `NAMETAG_ORG` | 13 |
| `NAMETAG_STATUS` | 14 |
| `NAMETAG_TAG_1` | 14 |
| `NAMETAG_TAG_2` | 7 |
| `NAMETAG_TAG_3` | 16 |
| `NAMETAG_TITLE` | 50 *(centered, full width)* |

Over budget? Shorten it, or widen the pill in `layout.h` — see below.

---

## Colors

### Using a built-in theme

One line in `config.h`:

```c
#define NAMETAG_THEME   THEME_MIDNIGHT
```

### Writing your own

A theme is twelve color slots. Copy an existing block in `theme.h`, rename it,
and register it:

```c
#define THEME_SUNSET  6          // near the top, with the others

#elif NAMETAG_THEME == THEME_SUNSET
  #define C_BG          0x2955
  #define C_CARD        0x39B8
  #define C_INK         0xFFDF
  #define C_ACCENT_A    0xFB46
  ...
```

Convert your hex colors first — RGB565 is not RGB888:

```bash
python3 tools/rgb565.py "#2d1b4e" "#ff6b35" "#f7c59f"
```

It also tells you what the panel will *actually* show, since 16-bit color loses
precision. Usually imperceptible; occasionally it matters for a brand color.

### What each slot does

| Slot | Where it lands |
|---|---|
| `C_BG` | Page behind the cards |
| `C_CARD` | The two card faces |
| `C_INK` | **Every outline and every piece of text.** The one that defines the look. |
| `C_ACCENT_A` | Header strip, org pill, tag 1, big sparkle |
| `C_ACCENT_B` | Avatar face, role pill |
| `C_ACCENT_C` | Avatar cheeks, tag 2, small sparkle |
| `C_ACCENT_D` | Status pill, tag 3 |
| `C_MUTED` | Footer text |
| `C_SHADOW` | Card drop shadow — set it to `C_BG` for a flat look |
| `C_HAIRLINE` | Thin divider rules |
| `C_DOT` | Background dot grid |
| `C_ON_ACCENT_C` | Text sitting on `C_ACCENT_C` — needs contrast against it |

Get `C_INK` and `C_BG` right and the rest follows. If the theme looks muddy,
that pair isn't far enough apart.

---

## Layout

Every position is a named macro in `layout.h`. Both the static render and the
animation loop read the same macros, so moving an element moves its animation
with it — that's the whole reason the file exists.

Widen the role pill for a long title:

```c
#define ROLE_PILL_W   160        // was 145
```

Move the sparkles to the top-right of the main card:

```c
#define SPARKLE_1_X   (CARD_R_X + CARD_R_W - 24)
#define SPARKLE_1_Y   (CARD_TOP + 20)          // was CARD_TOP + 120
```

Give the left card more room:

```c
#define CARD_L_W      120        // was 106
#define CARD_R_X      140        // was 126 — push the right card over to match
```

`CARD_R_W` is computed from `CARD_R_X`, so it adjusts on its own.

After any `layout.h` edit, run `./tools/verify.sh`. It compiles all five themes
in about a minute and catches the mistakes that only show up in one of them.

---

## Animations

Three independent toggles in `config.h`:

```c
#define ANIM_AVATAR_BLINK   true    // blinks and winks every 2.5-5s
#define ANIM_SPARKLE        true    // corner sparkles, 350ms per frame
#define ANIM_FLOATING       false   // status pill bobs +/-1px
```

All three on is still a comfortable 50fps. They're cheap because each one
repaints only its own small rectangle rather than redrawing the screen.

### Adding one

The pattern is always the same — a timer, a clear, a redraw. In `loop()`:

```c
#if ANIM_MY_THING
  static unsigned long last_tick = 0;
  if (now - last_tick > 500) {           // how often
    last_tick = now;
    gfx->fillRect(x, y, w, h, C_CARD);   // erase to whatever is behind it
    // ...draw the new frame
  }
#endif
```

Two things to get right: erase to the color that's actually behind the element
(`C_CARD` inside a card, `C_BG` on the page), and put its coordinates in
`layout.h` rather than typing numbers into `loop()`. Then declare the toggle in
`config.h` so it can be turned off.

---

## Images

See the README for the full walkthrough. Briefly:

```bash
python3 tools/prepare_image.py selfie.jpg --slot avatar
```

Copy `assets/sd-card/*` to a FAT32 card's root, then in `config.h`:

```c
#define IMAGE_SLOT_AVATAR   { true, "/avatar.png",  26,  46,  64,  64 }
//                            ^^^^ enabled
```

Images draw *on top of* the generated UI, in slot order — background first, then
avatar, sticker, icon. Turning on `IMAGE_SLOT_BACKGROUND` covers the cards
entirely, which is a legitimate way to build a fully custom badge: draw the whole
thing in Figma, export 320×240, and let the text pills sit on top.

Turn `NAMETAG_SHOW_AVATAR` off when you enable a photo avatar, or the drawn
smiley will sit underneath it.
