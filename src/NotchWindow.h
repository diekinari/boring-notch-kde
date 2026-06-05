#pragma once

#include <QObject>
#include <QRegion>

class QQuickWindow;
class QScreen;

// Configures the QML window as a wlr-layer-shell surface anchored to the top
// edge of the screen, centred horizontally, so it floats above normal windows
// like the macOS notch. This is the Wayland replacement for the macOS app's
// SkyLightWindow / CGSSpace / NotchSpaceManager machinery.
//
// On X11 this falls back to a frameless always-on-top window (handled in main).
class NotchWindow : public QObject {
    Q_OBJECT
public:
    explicit NotchWindow(QObject *parent = nullptr);

    // Promote an already-created QQuickWindow to a top-anchored layer surface
    // bound to the given output. Must be called before the window is shown.
    static void configureLayerShell(QQuickWindow *window, QScreen *screen);

    // Ask KWin to blur the desktop behind the (translucent) window — the
    // "frosted glass" part of the liquid-glass look. The region must match the
    // notch's rounded shape, otherwise the blur pokes past the rounded corners.
    // No-op off KDE/KWin.
    static void setGlass(QQuickWindow *window, bool enabled, const QRegion &region);

    // KWin background-contrast behind the window. The blur radius itself is a
    // global KWin setting and can't be set per-window, but contrast/intensity/
    // saturation can — this is our adjustable "frost strength". No-op off KWin.
    static void setFrost(QQuickWindow *window, bool enabled, qreal contrast,
                         qreal intensity, qreal saturation, const QRegion &region);
};
