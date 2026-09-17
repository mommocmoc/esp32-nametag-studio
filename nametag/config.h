/*
 * ==============================================================================
 *  config.h  —  THE ONLY FILE YOU NEED TO EDIT
 * ==============================================================================
 *  Everything that makes the badge *yours* lives here: your name, your role,
 *  your colors, your animations, your images.
 *
 *  Nothing in this file requires C++ knowledge. Change the text between the
 *  quotes, flip true/false, pick a theme. Then run:  ./tools/flash.sh
 *
 *  Working with a coding agent (Claude Code / Codex / Antigravity)?
 *  Just say: "Open config.h and make my badge say X" — the agent edits
 *  this file and nothing else.
 * ==============================================================================
 */

#pragma once

// ==============================================================================
//  1. WHO YOU ARE
// ==============================================================================

// The big line. ASCII / Latin characters render directly from the built-in font.
// For Korean, Japanese, Chinese, Cyrillic, Arabic, emoji — see section 2 below.
#define NAMETAG_NAME            "Cowcowwow"

// Small pill under your name. Your title, craft, or a joke. Keep it short.
#define NAMETAG_ROLE            "Maker & Tinkerer"

// Your team, company, lab, or crew. Shown on the yellow pill in the left card.
#define NAMETAG_ORG             "Cowcowwow"

// Your current mood / status. This is the fun one. Change it per event.
#define NAMETAG_STATUS          "OPEN TO CHAT"

// Top banner strip. Wrap it in asterisks for that sticker-sheet look.
#define NAMETAG_TITLE           "* HELLO, I MAKE THINGS *"

// Three sticker tags along the bottom of the main card.
// ~14 / ~8 / ~16 characters fit comfortably at the default sizes.
#define NAMETAG_TAG_1           "OPEN-SOURCE"
#define NAMETAG_TAG_2           "ESP32-S3"
#define NAMETAG_TAG_3           "SAY HI :)"

// Bottom-right footer note. Your handle, URL, or event name.
#define NAMETAG_FOOTER          "github.com/you"


// ==============================================================================
//  2. NON-LATIN NAMES (Korean / Japanese / Chinese / Cyrillic / emoji ...)
// ==============================================================================
//  The built-in font is ASCII-only. To show a name in any other script, render
//  it once into a bitmap on your computer, then flip the switch below.
//
//    python3 tools/make_name_bitmap.py "김민준"
//    python3 tools/make_name_bitmap.py "こんにちは" --size 26
//    python3 tools/make_name_bitmap.py "Ω ALEX Ω" --font /path/to/Font.ttf
//
//  That writes nametag/name_bitmap.h. Then set this to true:

#define USE_NAME_BITMAP         false

//  Tip: the bitmap is drawn at its native pixel size, top-left anchored inside
//  the main card. Keep it under 180 x 40 px so it fits. The generator warns you
//  if it doesn't.


// ==============================================================================
//  3. LOOK & FEEL
// ==============================================================================

//  Pick a theme. Definitions live in theme.h — add your own there.
//    THEME_KITSCH_POP  butter yellow + sage + peach   (default, playful)
//    THEME_MIDNIGHT    deep navy + cyan + magenta     (dark mode)
//    THEME_MONO_INK    paper white + black + one red  (editorial, high contrast)
//    THEME_CANDY       bubblegum pink + mint + lilac  (soft, sweet)
//    THEME_TERMINAL    black + phosphor green         (hacker)
#define NAMETAG_THEME           THEME_KITSCH_POP

//  Screen rotation.
//    1 = landscape 320x240  (default, best for a badge on a lanyard)
//    3 = landscape, flipped 180 degrees (use if your strap is on the other side)
//    0 / 2 = portrait 240x320 (the layout is designed for landscape — expect clipping)
#define NAMETAG_ROTATION        1

//  Backlight brightness, 0-100. Lower it to stretch battery life at an event.
#define NAMETAG_BRIGHTNESS      85

//  Draw the faint dotted grid in the background? Pure flat looks cleaner on
//  the dark themes.
#define NAMETAG_SHOW_DOT_GRID   true

//  Show the little smiley keyring avatar in the left card?
//  Turn this off if you're overlaying your own photo (section 5).
#define NAMETAG_SHOW_AVATAR     true


// ==============================================================================
//  4. ANIMATIONS
// ==============================================================================
//  Each one is independent. All three on is still a comfortable 50fps.

// The avatar blinks and winks every 2-5 seconds. Costs almost nothing.
#define ANIM_AVATAR_BLINK       true

// Two sparkles twinkle in the corner of the main card, 350ms per frame.
#define ANIM_SPARKLE            true

// The status pill bobs gently up and down.
#define ANIM_FLOATING           false


// ==============================================================================
//  5. YOUR OWN IMAGES (optional — needs a microSD card)
// ==============================================================================
//  Drop PNG or JPG files onto a FAT32-formatted microSD card, insert it, and
//  enable the slots below. PNG transparency is fully supported, which is what
//  you want for cut-out avatars and stickers.
//
//  Prepare files with:  python3 tools/prepare_image.py photo.jpg --slot avatar
//  (resizes + converts to exactly the right dimensions)
//
//  No card inserted? The badge renders the drawn UI instead. Nothing breaks.
//
//  Fields:  { enabled, "/path-on-sd", x, y, width, height }

#define IMAGE_SLOT_BACKGROUND   { false, "/bg.jpg",       0,   0, 320, 240 }
#define IMAGE_SLOT_AVATAR       { false, "/avatar.png",  26,  46,  64,  64 }
#define IMAGE_SLOT_STICKER      { false, "/sticker.png", 256,  36,  48,  48 }
#define IMAGE_SLOT_ICON         { false, "/icon.png",    264, 154,  36,  36 }


// ==============================================================================
//  6. HARDWARE — don't touch unless you changed boards
// ==============================================================================
//  Wired for the Waveshare ESP32-S3-Touch-LCD-2 (2.0" ST7789 IPS, 320x240).
//  Porting to another ST7789 board? These pins are the only thing to change.

#define LCD_PIN_SCLK   39
#define LCD_PIN_MOSI   38
#define LCD_PIN_MISO   40
#define LCD_PIN_DC     42
#define LCD_PIN_RST    -1
#define LCD_PIN_CS     45
#define LCD_PIN_BL      1

#define SD_PIN_CS      41

#define LCD_PANEL_W   240   // native panel width  (before rotation)
#define LCD_PANEL_H   320   // native panel height (before rotation)
