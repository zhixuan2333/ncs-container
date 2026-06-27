# OpenOCD configs

`host-bridge.sh` selects the probe and target through two env vars that map to
OpenOCD `-f` files:

```bash
OPENOCD_INTERFACE=interface/cmsis-dap.cfg   # your debug probe
OPENOCD_TARGET=target/nrf52.cfg             # your SoC
```

Both can be a name OpenOCD finds on its search path (`/usr/share/openocd/scripts`)
**or** a path to a file in this folder.

## Probes (`OPENOCD_INTERFACE`)
| Probe | Value |
|---|---|
| CMSIS-DAP (DAPLink, generic) | `interface/cmsis-dap.cfg` |
| SEGGER J-Link | `interface/jlink.cfg` |
| ST-Link | `interface/stlink.cfg` |
| FTDI-based | `interface/ftdi/<board>.cfg` |

## Targets (`OPENOCD_TARGET`)
| SoC | Value | Notes |
|---|---|---|
| nRF51 | `target/nrf51.cfg` | upstream |
| nRF52 | `target/nrf52.cfg` | upstream, well supported |
| nRF53 | `target/nrf53.cfg` | newer OpenOCD only; J-Link/pyOCD more reliable |
| nRF91 | `target/nrf91.cfg` | newer OpenOCD only |
| **nRF54L** (XIAO nRF54L15, default) | `target/nordic/nrf54l.cfg` | needs recent OpenOCD (see below) |

### nRF54L / XIAO nRF54L15
Recent OpenOCD ships full nRF54L support: `target/nordic/nrf54l.cfg` plus an
nRF54 NVM driver that programs the RRAM, so OpenOCD flashing works end-to-end —
this is the default for `host-bridge.sh`.

Requirement: an OpenOCD build new enough to include the nRF54 NVM driver
(upstream from late 2024 onward). Check with `openocd --version`; if your distro's
build is older, install a current OpenOCD (e.g. the xPack OpenOCD or Nordic's).

The bundled `openocd/nrf54l.cfg` is only a fallback for older OpenOCD without the
NVM driver — it gives halt / RAM debug but no RRAM flashing. Prefer the upstream
`target/nordic/nrf54l.cfg`.

The OpenOCD path is generic, so **any** OpenOCD-supported target works by just
changing `OPENOCD_TARGET`.
