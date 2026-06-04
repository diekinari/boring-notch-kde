#!/usr/bin/env bash
# Pull the latest changes, rebuild, and (re)install boring-notch-kde.
# Run this whenever you want to update to the newest version:
#   ./scripts/update.sh
set -euo pipefail

cd "$(dirname "$0")/.."   # repo root

echo "==> Pulling latest changes"
git pull --ff-only

echo "==> Configuring"
cmake -B build -DCMAKE_BUILD_TYPE=Release

echo "==> Building"
cmake --build build -j"$(nproc)"

echo "==> Installing"
# Install without sudo when the prefix is user-writable (e.g. ~/.local);
# fall back to sudo for system prefixes like /usr or /usr/local.
if ! cmake --install build 2>/dev/null; then
    echo "    (install prefix needs root — using sudo)"
    sudo cmake --install build
fi

echo "==> Done. Restart Boring Notch to run the new build."
