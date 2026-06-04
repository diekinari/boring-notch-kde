# Boring Notch — KDE / Arch Linux edition

A Linux/KDE reimplementation of [boring.notch](https://github.com/TheBoredTeam/boring.notch),
the macOS app that turns the MacBook notch into a dynamic media/info hub.

Linux laptops have no physical notch, so here it becomes a **floating overlay
pinned to the top edge** of the screen: a slim pill that expands on hover to show
media controls and (later) calendar, battery, a file shelf, an audio visualizer
and more.

> **Status: early scaffold / MVP.** Implemented so far:
> - Top-edge layer-shell overlay (Wayland) that expands on hover
> - Media controls via **MPRIS** over D-Bus (works with Spotify, browsers, VLC,
>   any MPRIS2 player) — play/pause, next/prev, title/artist/album art
> - **System-tray icon** + right-click context menu on the notch
> - **Settings window** with an Appearance page mirroring the macOS original,
>   persisted via QSettings (`~/.config/TheBoringTeam/boring-notch-kde.conf`).
>   Toggles for not-yet-built features (visualizer, tabs, face, Claude) are saved
>   and will activate once those features land.
>
> Not yet ported: audio visualizer, battery, calendar, webcam, file shelf, OSD,
> global shortcuts, X11 fallback. See [Roadmap](#roadmap).

This is **not** a port of the Swift source — SwiftUI/AppKit don't exist on Linux.
It's a ground-up rewrite in **Qt6/QML + KDE Frameworks**, reusing the macOS app's
ideas and feature set. Most system integrations map to clean Linux standards
(D-Bus / MPRIS / UPower / PipeWire) rather than the private macOS APIs the
original relies on.

## Requirements

- **Arch Linux** with **KDE Plasma 6** on a **Wayland** session
- Qt 6.5+
- `layer-shell-qt` (LayerShellQt) — for the top-edge overlay

Install the build/runtime deps on Arch:

```bash
sudo pacman -S --needed base-devel cmake \
    qt6-base qt6-declarative qt6-wayland \
    layer-shell-qt extra-cmake-modules
```

## Build & run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/boring-notch-kde
```

> The overlay relies on `wlr-layer-shell`. KWin (Plasma) supports it on Wayland.
> Running under X11 will currently fall back to a frameless window without
> top-edge anchoring (proper X11 support is on the roadmap).

## How to test (for testers)

You need **KDE Plasma 6 on a Wayland session** (check: System Settings →
About this System, or run `echo $XDG_SESSION_TYPE` — it should print `wayland`).
If it prints `x11`, log out and pick "Plasma (Wayland)" on the login screen.

1. Install deps and build (commands above). The build should finish with a
   `build/boring-notch-kde` binary.
2. Start any **MPRIS2** media player and begin playback — e.g. Spotify, a
   YouTube/SoundCloud tab in Firefox/Chromium, VLC, or `mpv`.
3. Run `./build/boring-notch-kde`.

**Expected result:**
- A slim black pill appears centred against the **top edge** of the screen,
  showing a small green dot + the current track title.
- **Hovering** the pill makes it grow downward into a card with album art,
  title/artist, and ⏮ ▶/⏸ ⏭ buttons.
- The play/pause and skip buttons control the running player; the title, artist
  and album art update when the track changes or you switch players.
- Moving the cursor away collapses it back to the pill.

**If something's off, please report:**
- Output of `echo $XDG_SESSION_TYPE` and `plasmashell --version`
- The full terminal output of `./build/boring-notch-kde` (it logs Qt/D-Bus
  warnings there)
- Whether the pill appears at all / appears but doesn't anchor to the top /
  appears but shows no track info (the last one usually means no MPRIS player
  is publishing — confirm with `playerctl metadata` if you have `playerctl`).

## Install

```bash
sudo cmake --install build
# autostart on login:
cp data/org.theboringteam.boringnotchkde.desktop ~/.config/autostart/
```

A PKGBUILD for the AUR lives in `packaging/` (WIP).

## Architecture

```
src/
  main.cpp          App entry: enables layer-shell, wires MprisManager into QML
  NotchWindow.*     Promotes the QML window to a top-anchored layer surface
  MprisManager.*    Discovers/tracks MPRIS players, picks the active one
  MprisPlayer.*     Wraps one MPRIS2 player (metadata + transport over D-Bus)
qml/
  Main.qml          The overlay window: collapsed pill <-> expanded card
  components/
    MusicView.qml   Now-playing card + transport controls
    MarqueeText.qml Scrolling text for long titles
```

### How macOS features map to Linux

| macOS (original)                         | Linux / KDE (here)                  |
| ---------------------------------------- | ----------------------------------- |
| SkyLight / CGSSpace notch window         | wlr-layer-shell (LayerShellQt)      |
| MediaRemote + Spotify/AppleMusic/YT ctrl | **MPRIS2** over D-Bus               |
| IOKit battery                            | UPower (D-Bus) — planned            |
| EventKit calendar                        | Akonadi / CalDAV — planned          |
| CoreAudio visualizer                     | PipeWire capture — planned          |
| AVFoundation webcam                      | V4L2 / PipeWire — planned           |
| KeyboardShortcuts                        | KGlobalAccel — planned              |
| Sparkle auto-update                      | pacman / AUR                        |

## Roadmap

1. ✅ Top-edge overlay + MPRIS media controls (MVP)
2. Seek bar + live position; player switcher when several are active
3. Battery indicator (UPower)
4. Calendar (Akonadi/CalDAV)
5. Audio visualizer (PipeWire)
6. File shelf with drag-and-drop
7. System OSD replacement (volume/brightness)
8. Webcam mirror (V4L2)
9. Global shortcuts (KGlobalAccel) + settings UI
10. X11 fallback, multi-monitor, AUR package

## License

GPLv3, inherited from the upstream boring.notch project. See [LICENSE](LICENSE).

Not affiliated with or endorsed by Apple. "MacBook" and "notch" refer to Apple
hardware; this project merely emulates the concept on Linux.
