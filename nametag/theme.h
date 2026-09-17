/*
 * ==============================================================================
 *  theme.h  —  color palettes
 * ==============================================================================
 *  Colors are RGB565 (16-bit). To convert a hex color:
 *      python3 tools/rgb565.py "#ff7e9e"      ->  0xFBEF
 *
 *  To add your own theme: copy a block, rename it, add it to the #if chain at
 *  the bottom, then set NAMETAG_THEME in config.h.
 *
 *  Ten slots make up a theme. The three you'll notice most are BG, ACCENT_A
 *  (the header strip) and INK (every outline and every piece of text).
 * ==============================================================================
 */

#pragma once

#define THEME_KITSCH_POP  1
#define THEME_MIDNIGHT    2
#define THEME_MONO_INK    3
#define THEME_CANDY       4
#define THEME_TERMINAL    5

// ------------------------------------------------------------------ 1. KITSCH POP
// Butter-milk paper, vivid yellow key color, sage/peach accents.
#if NAMETAG_THEME == THEME_KITSCH_POP
  #define C_BG          0xFFF7  // vanilla cream page
  #define C_CARD        0xFFFF  // pure white card
  #define C_INK         0x2124  // charcoal outline + text
  #define C_ACCENT_A    0xFFE0  // vivid yellow  (header, org pill, sparkle)
  #define C_ACCENT_B    0x8E14  // sage mint     (avatar face, role pill)
  #define C_ACCENT_C    0xFBEF  // peach pink    (cheeks, tag 2)
  #define C_ACCENT_D    0x7E3F  // sky blue      (status pill, tag 3)
  #define C_MUTED       0x7BEF  // warm grey footer text
  #define C_SHADOW      0xCE59  // card drop shadow
  #define C_HAIRLINE    0xDEFB  // thin divider rules
  #define C_DOT         0xE71C  // background dot grid
  #define C_ON_ACCENT_C 0xFFFF  // text color that sits on ACCENT_C

// ------------------------------------------------------------------ 2. MIDNIGHT
// Deep navy with electric cyan and magenta. Reads beautifully in a dim venue.
#elif NAMETAG_THEME == THEME_MIDNIGHT
  #define C_BG          0x0842
  #define C_CARD        0x18C3
  #define C_INK         0xE71C
  #define C_ACCENT_A    0x07FF  // cyan
  #define C_ACCENT_B    0x2D7F  // indigo
  #define C_ACCENT_C    0xF81F  // magenta
  #define C_ACCENT_D    0x07E0  // lime
  #define C_MUTED       0x738E
  #define C_SHADOW      0x0000
  #define C_HAIRLINE    0x31A6
  #define C_DOT         0x2124
  #define C_ON_ACCENT_C 0xFFFF

// ------------------------------------------------------------------ 3. MONO INK
// Newsprint. One red for emphasis, everything else black on white.
#elif NAMETAG_THEME == THEME_MONO_INK
  #define C_BG          0xF7BE
  #define C_CARD        0xFFFF
  #define C_INK         0x0000
  #define C_ACCENT_A    0xFFFF
  #define C_ACCENT_B    0xEF7D
  #define C_ACCENT_C    0xE0C3  // signal red
  #define C_ACCENT_D    0xDEDB
  #define C_MUTED       0x8410
  #define C_SHADOW      0xBDF7
  #define C_HAIRLINE    0xCE79
  #define C_DOT         0xDEDB
  #define C_ON_ACCENT_C 0xFFFF

// ------------------------------------------------------------------ 4. CANDY
// Bubblegum, mint and lilac. Soft outlines, high sweetness.
#elif NAMETAG_THEME == THEME_CANDY
  #define C_BG          0xFF9C
  #define C_CARD        0xFFFF
  #define C_INK         0x6188  // plum instead of black — softer
  #define C_ACCENT_A    0xFD1C  // bubblegum pink
  #define C_ACCENT_B    0xAFF5  // mint
  #define C_ACCENT_C    0xCCDF  // lilac
  #define C_ACCENT_D    0xFF37  // butter
  #define C_MUTED       0xA534
  #define C_SHADOW      0xE71C
  #define C_HAIRLINE    0xEF5D
  #define C_DOT         0xF71C
  #define C_ON_ACCENT_C 0x6188

// ------------------------------------------------------------------ 5. TERMINAL
// Black screen, phosphor green. No shadows, no softness.
#elif NAMETAG_THEME == THEME_TERMINAL
  #define C_BG          0x0000
  #define C_CARD        0x0000
  #define C_INK         0x07E0
  #define C_ACCENT_A    0x0320
  #define C_ACCENT_B    0x0000
  #define C_ACCENT_C    0x0320
  #define C_ACCENT_D    0x0000
  #define C_MUTED       0x0380
  #define C_SHADOW      0x0000
  #define C_HAIRLINE    0x01E0
  #define C_DOT         0x0160
  #define C_ON_ACCENT_C 0x07E0

#else
  #error "Unknown NAMETAG_THEME in config.h — pick one of THEME_KITSCH_POP / THEME_MIDNIGHT / THEME_MONO_INK / THEME_CANDY / THEME_TERMINAL"
#endif
