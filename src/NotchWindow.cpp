#include "NotchWindow.h"

#include <QQuickWindow>
#include <QScreen>

#include <KWindowEffects>
#include <LayerShellQt/Window>

NotchWindow::NotchWindow(QObject *parent) : QObject(parent) {}

void NotchWindow::configureLayerShell(QQuickWindow *window, QScreen *screen) {
    if (!window) return;

    // LayerShellQt::Window::get() attaches layer-shell role to the QWindow.
    auto *layer = LayerShellQt::Window::get(window);

    // Bind the surface to a specific output. The old setScreenConfiguration()
    // API (reading QWindow::screen()) is deprecated and ignored — every notch
    // ended up on the compositor's active screen. The current API is to set the
    // screen on the layer Window itself and opt out of "follow active screen".
    if (screen) layer->setScreen(screen);
    layer->setWantsToBeOnActiveScreen(false);

    // Top layer: above normal windows but below fullscreen-exclusive surfaces.
    layer->setLayer(LayerShellQt::Window::LayerTop);

    // Anchor to the top edge so the overlay hugs it like a notch. Anchoring to
    // top only (not left/right) keeps our explicit width and lets the
    // compositor centre us horizontally.
    layer->setAnchors(LayerShellQt::Window::AnchorTop);

    // Don't reserve screen space — we float over content, we don't push it.
    layer->setExclusiveZone(0);

    // We want hover/click on the overlay, but not to grab all keyboard focus.
    layer->setKeyboardInteractivity(
        LayerShellQt::Window::KeyboardInteractivityOnDemand);

    layer->setScope(QStringLiteral("boring-notch"));
}

void NotchWindow::setGlass(QQuickWindow *window, bool enabled,
                           const QRegion &region) {
    if (!window) return;
    // Region clips the blur to the rounded notch shape; without it KWin blurs
    // the full window rect and the desktop shows through past the corners.
    KWindowEffects::enableBlurBehind(window, enabled, region);
}

void NotchWindow::setFrost(QQuickWindow *window, bool enabled, qreal contrast,
                           qreal intensity, qreal saturation,
                           const QRegion &region) {
    if (!window) return;
    KWindowEffects::enableBackgroundContrast(window, enabled, contrast, intensity,
                                             saturation, region);
}
