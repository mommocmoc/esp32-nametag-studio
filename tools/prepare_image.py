#!/usr/bin/env python3
"""
prepare_image.py — get a photo ready for the badge's microSD card.

The badge draws images at their native size; it does not scale. So a 4000px
phone photo dropped in the avatar slot would spill across the whole screen.
This resizes to exactly the slot dimensions, and for the avatar it also cuts
a transparent circle so the photo sits properly inside the keyring frame.

USAGE
    python3 tools/prepare_image.py selfie.jpg --slot avatar
    python3 tools/prepare_image.py logo.png   --slot sticker
    python3 tools/prepare_image.py wall.jpg   --slot background
    python3 tools/prepare_image.py art.png    --size 100x80 --out /custom.png

Output lands in assets/sd-card/ — copy that folder's contents to the root of a
FAT32 microSD card, then enable the matching slot in nametag/config.h.

REQUIREMENTS
    pip3 install pillow
"""

import argparse
import os
import sys

# slot -> (width, height, filename, circular mask?)
SLOTS = {
    "background": (320, 240, "bg.jpg",      False),
    "avatar":     ( 64,  64, "avatar.png",  True),
    "sticker":    ( 48,  48, "sticker.png", False),
    "icon":       ( 36,  36, "icon.png",    False),
}


def main():
    ap = argparse.ArgumentParser(description="Resize an image to fit a badge image slot.")
    ap.add_argument("source", help="the image to convert")
    ap.add_argument("--slot", choices=SLOTS.keys(), help="which slot it's for")
    ap.add_argument("--size", help="custom WxH, e.g. 100x80 (instead of --slot)")
    ap.add_argument("--out", help="output path (default: assets/sd-card/<slot file>)")
    ap.add_argument("--no-circle", action="store_true",
                    help="skip the circular crop on the avatar slot")
    args = ap.parse_args()

    if not args.slot and not args.size:
        sys.exit("Pick one: --slot avatar|sticker|icon|background, or --size WxH")

    try:
        from PIL import Image, ImageDraw
    except ImportError:
        sys.exit("Pillow is required:  pip3 install pillow")

    if args.slot:
        w, h, name, circular = SLOTS[args.slot]
    else:
        try:
            w, h = (int(n) for n in args.size.lower().split("x"))
        except ValueError:
            sys.exit(f"--size must look like 100x80, got {args.size!r}")
        name, circular = "custom.png", False
    circular = circular and not args.no_circle

    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    out = args.out or os.path.join(root, "assets", "sd-card", name)
    os.makedirs(os.path.dirname(out), exist_ok=True)

    img = Image.open(args.source)

    # Cover-fit: fill the slot completely, cropping the overflow, so the
    # subject stays centered instead of being squashed.
    img = img.convert("RGBA")
    scale = max(w / img.width, h / img.height)
    img = img.resize((max(1, round(img.width * scale)),
                      max(1, round(img.height * scale))), Image.LANCZOS)
    left = (img.width - w) // 2
    top = (img.height - h) // 2
    img = img.crop((left, top, left + w, top + h))

    if circular:
        mask = Image.new("L", (w * 4, h * 4), 0)          # 4x for smooth edges
        ImageDraw.Draw(mask).ellipse((0, 0, w * 4, h * 4), fill=255)
        img.putalpha(mask.resize((w, h), Image.LANCZOS))

    if out.lower().endswith((".jpg", ".jpeg")):
        img.convert("RGB").save(out, quality=88, optimize=True)
    else:
        img.save(out, optimize=True)

    print(f"  in      {args.source}")
    print(f"  out     {out}  ({w}x{h}, {os.path.getsize(out) / 1024:.1f} KB)")
    if circular:
        print("  shape   circular, transparent corners")
    print()
    print("  Next:")
    print("   1. Copy assets/sd-card/* to the root of a FAT32 microSD card")
    print(f"   2. In nametag/config.h set the {args.slot or 'matching'} slot's first field to true")
    print("   3. ./tools/flash.sh")


if __name__ == "__main__":
    main()
