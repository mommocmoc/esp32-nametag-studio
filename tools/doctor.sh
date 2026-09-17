#!/usr/bin/env bash
#
# doctor.sh — check everything before you blame the hardware.
#
# Prints one line per requirement. Run it when a build or upload misbehaves,
# or paste its output to a coding agent and ask what's wrong.
#
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"
set +e   # a doctor reports problems, it doesn't die from them

bold "Nametag Studio — diagnostics"
echo

# --- toolchain ---
if command -v arduino-cli >/dev/null 2>&1; then
  ok "$(printf '%-24s' arduino-cli) $(arduino-cli version | awk '{print $3}')"
else
  warn "$(printf '%-24s' arduino-cli) MISSING — run ./tools/setup.sh"
fi

CORE="$(arduino-cli core list 2>/dev/null | awk '/^esp32:esp32/{print $2}')"
[ -n "$CORE" ] && ok "$(printf '%-24s' 'esp32 core') $CORE" \
                || warn "$(printf '%-24s' 'esp32 core') MISSING — run ./tools/setup.sh"

for lib in "GFX Library for Arduino" "PNGdec" "JPEGDEC"; do
  if arduino-cli lib list 2>/dev/null | grep -qi "^${lib}[[:space:]]"; then
    ok "$(printf '%-24s' "$lib") installed"
  else
    warn "$(printf '%-24s' "$lib") MISSING — run ./tools/setup.sh"
  fi
done

python3 -c "import PIL" >/dev/null 2>&1 \
  && ok "$(printf '%-24s' Pillow) installed" \
  || warn "$(printf '%-24s' Pillow) missing (only needed for non-Latin names)"

echo

# --- project files ---
for f in nametag/nametag.ino nametag/config.h nametag/theme.h nametag/layout.h; do
  [ -f "$ROOT/$f" ] && ok "$(printf '%-24s' "$(basename "$f")") present" \
                    || warn "$(printf '%-24s' "$(basename "$f")") MISSING"
done

if grep -q "USE_NAME_BITMAP *true" "$ROOT/nametag/config.h" 2>/dev/null; then
  [ -f "$ROOT/nametag/name_bitmap.h" ] \
    && ok "$(printf '%-24s' name_bitmap.h) present (USE_NAME_BITMAP is on)" \
    || warn "$(printf '%-24s' name_bitmap.h) MISSING but USE_NAME_BITMAP is true.
      Run: python3 tools/make_name_bitmap.py \"Your Name\"
      Or set USE_NAME_BITMAP back to false."
fi

echo

# --- board ---
PORT="$(detect_port)"
if [ -n "$PORT" ]; then
  ok "$(printf '%-24s' board) $PORT"
else
  warn "$(printf '%-24s' board) not detected
      Plug the board into a USB *data* cable, then:  arduino-cli board list
      If it still doesn't appear, hold BOOT, tap RESET, release BOOT."
fi

echo
bold "Board settings this project uses"
echo "  $FQBN"
echo
