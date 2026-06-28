# syntax=docker/dockerfile:1
#
# Prebuilt nRF Connect SDK dev container.
#
# Base = Nordic's official toolchain image (toolchain only, no SDK source).
# We bake the matching sdk-nrf source tree into the image with `west update`
# so a Codespace starts ready-to-build with no network fetch.
#
# The image is published to GHCR with the SAME tag as the NCS version, e.g.
#   ghcr.io/<owner>/ncs-container:v3.3.0

ARG NCS_VERSION=v3.3.0
FROM ghcr.io/nrfconnect/sdk-nrf-toolchain:${NCS_VERSION}

# Re-declare after FROM so it is visible in the build stage.
ARG NCS_VERSION=v3.3.0
ENV NCS_VERSION=${NCS_VERSION} \
    NCS_WORKSPACE=/opt/ncs/${NCS_VERSION} \
    ZEPHYR_BASE=/opt/ncs/${NCS_VERSION}/zephyr \
    DEBIAN_FRONTEND=noninteractive

# Extra tooling for the "network OpenOCD/serial" hardware bridge and on-target
# debugging from inside the Codespace:
#   openocd           - so the same cfgs work locally too / client convenience
#   socat, netcat     - serial<->TCP bridging and quick port checks
#   tio               - friendly serial terminal (connects to the forwarded port)
#   gdb-multiarch     - fallback debugger (arm-zephyr-eabi-gdb ships in toolchain)
#   openssh-client    - reverse tunnel from the local host terminates here
RUN apt-get update && apt-get install -y --no-install-recommends \
        openocd \
        socat \
        netcat-openbsd \
        tio \
        gdb-multiarch \
        openssh-client \
        openssh-server \
        telnet \
        less \
        nano \
        sudo \
    && rm -rf /var/lib/apt/lists/*

# ---------------------------------------------------------------------------
# Bake the NCS source tree (prebuild).
# Shallow + narrow keeps the image as small as practical while still building.
# ---------------------------------------------------------------------------
RUN set -eux; \
    source /opt/toolchain-env.sh; \
    mkdir -p "${NCS_WORKSPACE}"; \
    west init -m https://github.com/nrfconnect/sdk-nrf --mr "${NCS_VERSION}" "${NCS_WORKSPACE}"; \
    cd "${NCS_WORKSPACE}"; \
    west update --narrow -o=--depth=1; \
    west zephyr-export; \
    # trim caches to shrink the layer
    rm -rf "${NCS_WORKSPACE}/.west/.cache" || true; \
    find "${NCS_WORKSPACE}" -name '.git' -type d -prune -exec du -sh {} + >/dev/null 2>&1 || true

WORKDIR /workspaces

# Make every bash session find the toolchain (BASH_ENV already sources it, but
# this also covers `docker exec` and login shells used by VS Code).
RUN printf '\n# nRF Connect SDK toolchain\nsource /opt/toolchain-env.sh 2>/dev/null || true\nexport ZEPHYR_BASE=%s\n' "${ZEPHYR_BASE}" >> /opt/.bashrc

LABEL org.opencontainers.image.title="ncs-container" \
      org.opencontainers.image.description="Prebuilt nRF Connect SDK dev container (Codespaces + OpenOCD/serial bridge)"
