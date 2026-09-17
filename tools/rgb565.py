#!/usr/bin/env python3
"""
rgb565.py — turn a normal hex color into the 16-bit value theme.h wants.

The display stores 5 bits of red, 6 of green, 5 of blue. Your design tool's
#rrggbb has to be squeezed into that, and doing it by hand is miserable.

USAGE
    python3 tools/rgb565.py "#ff7e9e"
    python3 tools/rgb565.py ff7e9e 88c2a3 222428      # several at once
    python3 tools/rgb565.py 0xFBEF --reverse          # go back to hex

No dependencies.
"""

import argparse
import sys


def to565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)


def from565(v):
    r = (v >> 11) & 0x1F
    g = (v >> 5) & 0x3F
    b = v & 0x1F
    # Replicate high bits into the low ones so white stays white.
    return (r << 3) | (r >> 2), (g << 2) | (g >> 4), (b << 3) | (b >> 2)


def parse_hex(s):
    s = s.lstrip("#").strip()
    if len(s) == 3:
        s = "".join(c * 2 for c in s)
    if len(s) != 6:
        sys.exit(f"Not a 6-digit hex color: {s!r}")
    return int(s[0:2], 16), int(s[2:4], 16), int(s[4:6], 16)


def main():
    ap = argparse.ArgumentParser(description="Convert hex colors to RGB565 and back.")
    ap.add_argument("colors", nargs="+", help="#rrggbb values, or 0x#### with --reverse")
    ap.add_argument("--reverse", action="store_true", help="RGB565 -> hex")
    args = ap.parse_args()

    for c in args.colors:
        if args.reverse:
            v = int(c, 16) if c.lower().startswith("0x") else int(c, 16)
            r, g, b = from565(v)
            print(f"  0x{v:04X}  ->  #{r:02x}{g:02x}{b:02x}")
        else:
            r, g, b = parse_hex(c)
            v = to565(r, g, b)
            # Show what the panel will actually display — 565 loses precision.
            rr, gg, bb = from565(v)
            note = "" if (rr, gg, bb) == (r, g, b) else f"   (shows as #{rr:02x}{gg:02x}{bb:02x})"
            print(f"  #{r:02x}{g:02x}{b:02x}  ->  0x{v:04X}{note}")


if __name__ == "__main__":
    main()
