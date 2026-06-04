#pragma once

#include <QObject>

class QQuickWindow;

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

    // Promote an already-created QQuickWindow to a top-anchored layer surface.
    // Must be called before the window is first shown.
    static void configureLayerShell(QQuickWindow *window);
};
