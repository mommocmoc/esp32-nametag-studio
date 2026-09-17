# assets/sd-card/

What goes on the microSD card.

`tools/prepare_image.py` writes here. Copy the contents — not the folder — to
the **root** of a FAT32-formatted card:

```
SD card root/
  avatar.png
  sticker.png
  bg.jpg
```

Then enable the matching slot in `nametag/config.h` §5 by flipping its first
field to `true`.

## Sizes

| Slot | Dimensions | Filename | Notes |
|---|---|---|---|
| Background | 320×240 | `bg.jpg` | Covers everything. JPEG decodes faster. |
| Avatar | 64×64 | `avatar.png` | PNG with alpha; `prepare_image.py` cuts the circle |
| Sticker | 48×48 | `sticker.png` | Top-right corner |
| Icon | 36×36 | `icon.png` | Bottom-right corner |

Images are drawn at native size — nothing is scaled on the device. A photo
straight off a phone will paint across the entire screen.

```bash
python3 tools/prepare_image.py ~/Desktop/selfie.jpg --slot avatar
```

Format the card **FAT32**. exFAT will not mount.
