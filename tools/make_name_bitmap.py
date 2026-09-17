#!/usr/bin/env python3
"""
make_name_bitmap.py — render a name in ANY script into a C header for the badge.

The ESP32's built-in font is ASCII-only. Korean, Japanese, Chinese, Cyrillic,
Greek, Thai, Arabic, emoji — none of it will render from a string. So we render
the text to pixels here, on your computer, where the real fonts live, and ship
the result as a 1-bit bitmap.

USAGE
    python3 tools/make_name_bitmap.py "김민준"
    python3 tools/make_name_bitmap.py "こんにちは" --size 26
    python3 tools/make_name_bitmap.py "ALEX" --font ~/Library/Fonts/Impact.ttf
    python3 tools/make_name_bitmap.py "Ω LOUD Ω" --size 30 --preview

Then set  USE_NAME_BITMAP  to  true  in nametag/config.h.

REQUIREMENTS
    pip3 install pillow
"""

import argparse
import os
import platform
import sys

MAX_W = 180   # the name area inside the right card
MAX_H = 40

# Fonts that ship with the OS and cover a wide range of scripts.
FONT_CANDIDATES = {
    "Darwin": [
        "/System/Library/Fonts/AppleSDGothicNeo.ttc",      # Korean
        "/System/Library/Fonts/Hiragino Sans GB.ttc",      # Japanese / Chinese
        "/System/Library/Fonts/PingFang.ttc",              # Chinese
        "/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
    ],
    "Linux": [
        "/usr/share/fonts/truetype/noto/NotoSansCJK-Bold.ttc",
        "/usr/share/fonts/opentype/noto/NotoSansCJK-Bold.ttc",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf",
    ],
    "Windows": [
        "C:/Windows/Fonts/malgunbd.ttf",                   # Korean
        "C:/Windows/Fonts/msgothic.ttc",                   # Japanese
        "C:/Windows/Fonts/msyhbd.ttc",                     # Chinese
        "C:/Windows/Fonts/arialbd.ttf",
    ],
}


def pick_font(explicit, size):
    from PIL import ImageFont

    if explicit:
        path = os.path.expanduser(explicit)
        if not os.path.exists(path):
            sys.exit(f"Font not found: {path}")
        return ImageFont.truetype(path, size), path

    for path in FONT_CANDIDATES.get(platform.system(), []):
        if os.path.exists(path):
            try:
                return ImageFont.truetype(path, size), path
            except OSError:
                continue

    sys.exit(
        "No usable system font found. Pass one explicitly:\n"
        "    --font /path/to/YourFont.ttf\n"
        "On Linux try:  sudo apt install fonts-noto-cjk"
    )


def render(text, font, threshold):
    from PIL import Image, ImageDraw

    # Measure on a scratch canvas, then draw tight to the ink bounds so the
    # bitmap has no wasted margin (every byte counts on the device).
    probe = Image.new("L", (1, 1), 0)
    box = ImageDraw.Draw(probe).textbbox((0, 0), text, font=font)
    w, h = box[2] - box[0], box[3] - box[1]
    if w <= 0 or h <= 0:
        sys.exit(f"The font rendered nothing for {text!r} — it likely lacks those glyphs.")

    canvas = Image.new("L", (w + 4, h + 4), 0)
    ImageDraw.Draw(canvas).text((-box[0] + 2, -box[1] + 2), text, font=font, fill=255)

    mono = canvas.point(lambda p: 255 if p >= threshold else 0, mode="1")
    bbox = mono.getbbox()
    return mono.crop(bbox) if bbox else mono


def pack(img):
    """Row-major, MSB-first, each row padded to a byte — what Arduino_GFX
    drawBitmap() expects."""
    w, h = img.size
    stride = (w + 7) // 8
    px = img.load()
    out = bytearray()
    for y in range(h):
        for bx in range(stride):
            byte = 0
            for bit in range(8):
                x = bx * 8 + bit
                if x < w and px[x, y]:
                    byte |= 0x80 >> bit
            out.append(byte)
    return bytes(out), stride


def preview(img):
    w, h = img.size
    px = img.load()
    print(f"\n  preview ({w}x{h}):")
    for y in range(h):
        print("  " + "".join("#" if px[x, y] else "." for x in range(w)))
    print()


def write_header(path, text, img, data, font_path, size):
    w, h = img.size
    lines = [
        "/*",
        " * name_bitmap.h — GENERATED FILE, do not hand-edit.",
        " *",
        f" *   text : {text}",
        f" *   font : {font_path}",
        f" *   size : {size}px  ->  {w}x{h} px, {len(data)} bytes",
        " *",
        " * Regenerate:",
        f' *   python3 tools/make_name_bitmap.py "{text}" --size {size}',
        " *",
        " * Remember to set USE_NAME_BITMAP to true in config.h.",
        " */",
        "",
        "#pragma once",
        "",
        f"#define NAME_BITMAP_W {w}",
        f"#define NAME_BITMAP_H {h}",
        "",
        "const unsigned char NAME_BITMAP[] PROGMEM = {",
    ]
    for i in range(0, len(data), 12):
        chunk = ", ".join(f"0x{b:02X}" for b in data[i:i + 12])
        lines.append(f"  {chunk},")
    lines += ["};", ""]

    with open(path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))


def main():
    ap = argparse.ArgumentParser(
        description="Render a name into a 1-bit C bitmap header for Nametag Studio.")
    ap.add_argument("text", help="the name to render, in any script")
    ap.add_argument("--size", type=int, default=28, help="font size in px (default: 28)")
    ap.add_argument("--font", help="path to a .ttf / .otf / .ttc file")
    ap.add_argument("--out", help="output header (default: nametag/name_bitmap.h)")
    ap.add_argument("--threshold", type=int, default=128,
                    help="1-bit cutoff, 0-255. Lower = bolder (default: 128)")
    ap.add_argument("--preview", action="store_true",
                    help="print the bitmap as ASCII art so you can check it")
    args = ap.parse_args()

    try:
        import PIL  # noqa: F401
    except ImportError:
        sys.exit("Pillow is required:  pip3 install pillow")

    font, font_path = pick_font(args.font, args.size)
    img = render(args.text, font, args.threshold)
    data, _ = pack(img)
    w, h = img.size

    if args.preview:
        preview(img)

    out = args.out or os.path.join(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
        "nametag", "name_bitmap.h")
    write_header(out, args.text, img, data, font_path, args.size)

    print(f"  text    {args.text}")
    print(f"  font    {font_path}")
    print(f"  size    {w} x {h} px  ({len(data)} bytes)")
    print(f"  wrote   {out}")

    if w > MAX_W or h > MAX_H:
        print(f"\n  ! {w}x{h} is larger than the {MAX_W}x{MAX_H} name area "
              f"and will be clipped.\n    Try:  --size {max(8, int(args.size * MAX_W / w))}")
    else:
        print("\n  Next: set USE_NAME_BITMAP to true in nametag/config.h, then ./tools/flash.sh")


if __name__ == "__main__":
    main()
