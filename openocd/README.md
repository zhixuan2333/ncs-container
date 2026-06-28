# OpenOCD configs

`host-bridge.sh` runs OpenOCD in one of two modes.

**Board-cfg mode (default).** A single self-contained `.cfg` that wires up probe +
target itself. The default is [`xiao_nrf54l15.cfg`](xiao_nrf54l15.cfg) (vendored
from the Zephyr board support files), which works with **stock OpenOCD 0.12.0** —
it does *not* need `target/nordic/nrf54l.cfg`. The probe is chosen by a bare
`OPENOCD_INTERFACE` name:

```bash
OPENOCD_INTERFACE=cmsis-dap                 # bare probe name
OPENOCD_CFG=openocd/xiao_nrf54l15.cfg       # default
```

**Two-file mode.** Set `OPENOCD_TARGET` to use the classic interface + target pair
for any other OpenOCD-supported SoC:

```bash
OPENOCD_INTERFACE=cmsis-dap                 # bare name, or a full interface/*.cfg path
OPENOCD_TARGET=target/nrf52.cfg             # your SoC
```

`OPENOCD_TARGET` can be a name on OpenOCD's search path (`/usr/share/openocd/scripts`)
or a path to a file in this folder.

## Probes (`OPENOCD_INTERFACE`)
| Probe | Bare name |
|---|---|
| CMSIS-DAP (DAPLink, XIAO onboard) | `cmsis-dap` |
| SEGGER J-Link | `jlink` |
| ST-Link | `stlink` |

## Targets (`OPENOCD_TARGET`)
| SoC | Value | Notes |
|---|---|---|
| nRF51 | `target/nrf51.cfg` | upstream |
| nRF52 | `target/nrf52.cfg` | upstream, well supported |
| nRF53 | `target/nrf53.cfg` | newer OpenOCD only; J-Link/pyOCD more reliable |
| nRF91 | `target/nrf91.cfg` | newer OpenOCD only |
| **nRF54L** (XIAO nRF54L15, default) | board-cfg `openocd/xiao_nrf54l15.cfg` | works with OpenOCD 0.12.0 |

### nRF54L / XIAO nRF54L15
The default uses the bundled self-contained [`xiao_nrf54l15.cfg`](xiao_nrf54l15.cfg)
(board-cfg mode) instead of `target/nordic/nrf54l.cfg`, so it works with **stock
OpenOCD 0.12.0** — no recent OpenOCD build required. Verified against real
hardware: it detects the Cortex-M33 over CMSIS-DAP and programs RRAM.

RRAM programming needs write-enable (RRAMC `CONFIG.WEN`) before writing. When you
flash from a Codespace, [`scripts/flash.sh`](../scripts/flash.sh) issues that
(`monitor mww 0x5004b500 0x101`, via `OPENOCD_PRELOAD`) automatically before
`load`. The cfg's `nrf54l-load` proc does the same for local host-side flashing.

The OpenOCD path is generic, so **any** OpenOCD-supported target works by setting
`OPENOCD_TARGET` (two-file mode) or pointing `OPENOCD_CFG` at another board cfg.
