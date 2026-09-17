#!/usr/bin/env bash
#
# build.sh — compile the badge. No board needed.
#
#   ./tools/build.sh              compile
#   ./tools/build.sh --clean      compile from scratch
#
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"
need_cli

[ "${1:-}" = "--clean" ] && { info "clearing $BUILD_DIR"; rm -rf "$BUILD_DIR"; }

bold "Compiling nametag.ino"
info "board: $FQBN"
echo

arduino-cli compile \
  --fqbn "$FQBN" \
  --build-path "$BUILD_DIR" \
  --warnings default \
  "$SKETCH"

echo
ok "Build succeeded"
BIN="$BUILD_DIR/nametag.ino.bin"
[ -f "$BIN" ] && info "firmware: $BIN ($(du -h "$BIN" | cut -f1))"
echo
echo "  Next: ./tools/flash.sh"
