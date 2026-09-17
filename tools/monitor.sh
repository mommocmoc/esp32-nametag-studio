#!/usr/bin/env bash
#
# monitor.sh — watch the badge's serial output. Ctrl-C to quit.
#
# Useful when something looks wrong: the board reports display init, microSD
# mount status, and every image it tries to load.
#
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"
need_cli

PORT="${1:-}"
[ -z "$PORT" ] && { PORT="$(detect_port)" || die "No board found. Is it plugged in?"; }

bold "Monitoring $PORT at ${BAUD} baud — Ctrl-C to quit"
echo
arduino-cli monitor --port "$PORT" --config "baudrate=$BAUD"
