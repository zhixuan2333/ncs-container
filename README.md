# ncs-container

A prebuilt **nRF Connect SDK (NCS) v3.3.0** dev container for **GitHub Codespaces**,
with on-target **OpenOCD** flashing/debugging and **serial console** access to a
physical board on your desk — even though the Codespace runs in the cloud.

- 🐳 Prebuilt image published to **GHCR**, tagged to match the NCS version
  (`ghcr.io/zhixuan2333/ncs-container:v3.3.0`).
- 🔌 **OpenOCD** GDB flash/debug from the Codespace to your real device.
- 🖥️ **Serial** console forwarded into the Codespace.
- 🎯 Works with the **Seeed XIAO nRF54L15** and **any OpenOCD-supported target**
  (configurable probe + target).

---

## How hardware reaches the cloud

Codespaces have no USB. We use the **network OpenOCD/serial** model: tools run on
your **local machine** and a **reverse SSH tunnel** exposes them inside the Codespace.

```
Local machine (board plugged in)          GitHub Codespace (cloud)
┌──────────────────────────────┐          ┌──────────────────────────────┐
│ openocd  gdb:3333 telnet:4444 │          │ arm-zephyr-eabi-gdb → :3333   │
│ socat  serial ⇄ tcp:4555      │ gh ssh   │ west build / ./scripts/flash  │
│ gh codespace ssh -R 3333,...  │═════════▶│ ./scripts/serial → :4555      │
└──────────────────────────────┘ reverse  └──────────────────────────────┘
                                  tunnel
```

`localhost:{3333,4444,4555}` *inside* the Codespace map back to OpenOCD and the
serial bridge running *on your machine*.

---

## One-time setup

### 1. Publish the prebuilt image
Push this repo to GitHub. The workflow `.github/workflows/publish-image.yml`
builds and pushes the image on every change to `.devcontainer/**`, or run it
manually:

```bash
gh workflow run "Publish dev container image" -f ncs_version=v3.3.0
```

Then make the GHCR package accessible to Codespaces:
- GitHub → your profile → **Packages** → `ncs-container` → **Package settings**
- Set visibility to **Public**, *or* under **Manage Codespaces access** grant this repo.

> To target a different NCS release later, run the workflow with another
> `ncs_version` and update the tag in `.devcontainer/devcontainer.json`.

### 2. Local machine prerequisites
Install on the host where the board is plugged in:

```bash
# macOS
brew install gh openocd socat
# Debian/Ubuntu
sudo apt install gh openocd socat
gh auth login
```

You also need a **debug probe** (CMSIS-DAP / J-Link / ST-Link) wired to the SoC's SWD.

---

## Daily workflow

### 1. Open the Codespace
Create it from this repo (**Code → Codespaces → Create**). It pulls the prebuilt
image — the NCS source is already baked in at `/opt/ncs/v3.3.0`.

### 2. Build (inside the Codespace)
```bash
west build -b nrf52840dk/nrf52840 app      # or your own board / app
# Seeed XIAO nRF54L15:  west build -b xiao_nrf54l15/nrf54l15/cpuapp app
```

### 3. Connect the board (on your LOCAL machine)
```bash
./scripts/host-bridge.sh <codespace-name>     # defaults to XIAO nRF54L15 + CMSIS-DAP
# override probe/target for another board:
OPENOCD_INTERFACE=interface/cmsis-dap.cfg \
OPENOCD_TARGET=target/nrf52.cfg \
  ./scripts/host-bridge.sh <codespace-name>
```
Find the name with `gh codespace list`. Leave this running (Ctrl-C to stop).

### 4. Flash / debug / serial (inside the Codespace)
```bash
./scripts/flash.sh                 # flash build/zephyr/zephyr.elf over OpenOCD
./scripts/debug.sh                 # interactive arm-zephyr-eabi-gdb session
./scripts/serial.sh                # open the forwarded serial console
```

---

## Choosing your target

`host-bridge.sh` is generic. Set the probe/target via env vars — see
[`openocd/README.md`](openocd/README.md) for the full table.

| What | Env var | Example |
|---|---|---|
| Debug probe | `OPENOCD_INTERFACE` | `interface/cmsis-dap.cfg` |
| Target SoC | `OPENOCD_TARGET` | `target/nordic/nrf54l.cfg` (default) |
| Serial port | `SERIAL_PORT` | `/dev/tty.usbmodem1101` (auto-detected if unset) |
| Baud | `SERIAL_BAUD` | `115200` |

### XIAO nRF54L15 (default)
`host-bridge.sh` defaults to `interface/cmsis-dap.cfg` + `target/nordic/nrf54l.cfg`,
so OpenOCD flashes/debugs the XIAO nRF54L15 out of the box. This needs a recent
OpenOCD that includes the nRF54 NVM/RRAM driver (upstream late-2024+); check
`openocd --version` and install a current build if needed. Details and a fallback
config in [`openocd/README.md`](openocd/README.md).

---

## Layout

```
.devcontainer/
  Dockerfile          # base = nrfconnect/sdk-nrf-toolchain:<ver> + baked SDK
  devcontainer.json   # points at the prebuilt GHCR image
  post-create.sh      # prints quick-start, sets up PATH/git
.github/workflows/
  publish-image.yml   # build & push image to GHCR, tag == NCS version
scripts/
  host-bridge.sh      # LOCAL: openocd + serial bridge + reverse tunnel
  flash.sh            # CODESPACE: flash ELF via tunneled OpenOCD
  debug.sh            # CODESPACE: interactive gdb
  serial.sh           # CODESPACE: serial console over the bridge
openocd/
  nrf54l.cfg          # generic nRF54L config + README with probe/target table
app/                  # minimal blinky+printk sample to verify the chain
```

---

## Build the image locally (optional)

You don't need the published image to develop — flip `.devcontainer/devcontainer.json`
to the `build` block, or:

```bash
docker build -t ncs:v3.3.0 --build-arg NCS_VERSION=v3.3.0 -f .devcontainer/Dockerfile .
```
