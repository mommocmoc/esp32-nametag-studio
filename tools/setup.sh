#!/usr/bin/env bash
#
# setup.sh — one command to go from a blank machine to a buildable badge.
#
# Installs arduino-cli (if missing), the ESP32 board core, and the four
# libraries the sketch needs. Safe to run more than once: everything here
# skips work that's already done.
#
#   ./tools/setup.sh
#
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

bold "Nametag Studio — environment setup"
echo

# ---------------------------------------------------------------- arduino-cli
if command -v arduino-cli >/dev/null 2>&1; then
  ok "arduino-cli $(arduino-cli version | awk '{print $3}')"
else
  info "installing arduino-cli..."
  if command -v brew >/dev/null 2>&1; then
    brew install arduino-cli
  else
    mkdir -p "$HOME/.local/bin"
    curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh \
      | BINDIR="$HOME/.local/bin" sh
    export PATH="$HOME/.local/bin:$PATH"
    warn "Installed to ~/.local/bin — add it to your PATH:"
    echo '      echo '"'"'export PATH="$HOME/.local/bin:$PATH"'"'"' >> ~/.zshrc'
  fi
  command -v arduino-cli >/dev/null 2>&1 || die "arduino-cli install failed"
  ok "arduino-cli installed"
fi

arduino-cli config init --overwrite >/dev/null 2>&1 || true

# ------------------------------------------------------------------ ESP32 core
ESP32_URL="https://espressif.github.io/arduino-esp32/package_esp32_index.json"
info "registering the ESP32 board index..."
arduino-cli config add board_manager.additional_urls "$ESP32_URL" >/dev/null 2>&1 \
  || arduino-cli config set board_manager.additional_urls "$ESP32_URL" >/dev/null 2>&1
arduino-cli core update-index >/dev/null 2>&1

if arduino-cli core list 2>/dev/null | grep -q "^esp32:esp32"; then
  ok "ESP32 core $(arduino-cli core list | awk '/^esp32:esp32/{print $2}')"
else
  info "installing the ESP32 core (this pulls ~1GB of toolchain — go make coffee)"
  arduino-cli core install esp32:esp32
  ok "ESP32 core installed"
fi

# ------------------------------------------------------------------- libraries
# GFX  : the ST7789 display driver and all drawing primitives
# PNGdec/JPEGDEC : decode artwork off the microSD card
# BusIO/Adafruit GFX : transitive dependencies of the GFX library
LIBS=("GFX Library for Arduino" "PNGdec" "JPEGDEC" "Adafruit GFX Library" "Adafruit BusIO")
for lib in "${LIBS[@]}"; do
  if arduino-cli lib list 2>/dev/null | grep -qi "^${lib}[[:space:]]"; then
    ok "$lib"
  else
    info "installing $lib..."
    arduino-cli lib install "$lib" >/dev/null
    ok "$lib installed"
  fi
done

# ---------------------------------------------------------------------- Pillow
# Only needed if you want a non-Latin name. Not fatal if it's missing.
if python3 -c "import PIL" >/dev/null 2>&1; then
  ok "Pillow (for non-Latin name bitmaps)"
else
  warn "Pillow not installed — needed only for make_name_bitmap.py"
  echo "      pip3 install pillow"
fi

echo
bold "Ready."
echo
echo "  1. Edit  nametag/config.h   — your name, role, colors"
echo "  2. Run   ./tools/build.sh   — check it compiles"
echo "  3. Run   ./tools/flash.sh   — plug in the board and send it"
echo
