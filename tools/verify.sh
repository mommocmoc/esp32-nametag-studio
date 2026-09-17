#!/usr/bin/env bash
#
# verify.sh — compile every theme and both name modes.
#
# Run this after changing anything in nametag/ that isn't just text. It catches
# the classic "looks fine, breaks on another theme" mistake in about a minute.
# Your config.h is restored exactly as it was, even if a build fails.
#
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"
need_cli
set +e

CFG="$SKETCH/config.h"
BAK="$(mktemp)"
cp "$CFG" "$BAK"
restore() { cp "$BAK" "$CFG"; rm -f "$BAK"; }
trap restore EXIT INT TERM

# sed -i differs between GNU and BSD; this works on both.
edit() { sed -i.tmp "$1" "$CFG" && rm -f "$CFG.tmp"; }

fails=0
try() {
  if arduino-cli compile --fqbn "$FQBN" --build-path "$BUILD_DIR" "$SKETCH" >/dev/null 2>&1; then
    ok "$1"
  else
    warn "$1  — FAILED"
    fails=$((fails + 1))
  fi
}

bold "Verifying every configuration"
echo

for t in THEME_KITSCH_POP THEME_MIDNIGHT THEME_MONO_INK THEME_CANDY THEME_TERMINAL; do
  edit "s/^#define NAMETAG_THEME .*/#define NAMETAG_THEME $t/"
  try "theme $t"
done

cp "$BAK" "$CFG"
if [ -f "$SKETCH/name_bitmap.h" ]; then
  edit "s/^#define USE_NAME_BITMAP .*/#define USE_NAME_BITMAP true/"
  try "USE_NAME_BITMAP true"
else
  info "skipping bitmap mode — no name_bitmap.h yet"
fi

cp "$BAK" "$CFG"
edit "s/^#define ANIM_FLOATING .*/#define ANIM_FLOATING true/"
try "all three animations on"

cp "$BAK" "$CFG"
edit "s/^#define NAMETAG_SHOW_AVATAR .*/#define NAMETAG_SHOW_AVATAR false/"
edit "s/^#define ANIM_AVATAR_BLINK .*/#define ANIM_AVATAR_BLINK false/"
try "avatar hidden"

echo
if [ "$fails" -eq 0 ]; then
  bold "All configurations build."
else
  die "$fails configuration(s) failed to build."
fi
