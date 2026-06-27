#!/usr/bin/env bash
# Runs once after the Codespace/container is created.
set -euo pipefail

source /opt/toolchain-env.sh 2>/dev/null || true

# Trust the mounted workspace for git.
git config --global --add safe.directory "*" || true

# Make the helper scripts executable + on PATH for this session.
chmod +x /workspaces/ncs-container/scripts/*.sh 2>/dev/null || true

cat <<EOF

  nRF Connect SDK dev container ready.
  ────────────────────────────────────────────────────────────
  NCS source : $NCS_WORKSPACE        (zephyr at $ZEPHYR_BASE)
  west       : $(command -v west)
  gdb        : arm-zephyr-eabi-gdb

  Build the sample app:
      west build -b <board> app        # e.g. -b nrf52840dk/nrf52840

  Connect hardware (run on your LOCAL machine, board plugged in):
      ./scripts/host-bridge.sh <codespace-name>

  Then, inside this Codespace:
      ./scripts/flash.sh               # flash build/zephyr/zephyr.elf via OpenOCD
      ./scripts/debug.sh               # interactive gdb session
      ./scripts/serial.sh              # open the forwarded serial console
  ────────────────────────────────────────────────────────────
EOF
