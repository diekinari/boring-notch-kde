#include "NotchWindow.h"

#include <QQuickWindow>

#include <LayerShellQt/Window>

NotchWindow::NotchWindow(QObject *parent) : QObject(parent) {}

void NotchWindow::configureLayerShell(QQuickWindow *window) {
    if (!window) return;

    // LayerShellQt::Window::get() attaches layer-shell role to the QWindow.
    auto *layer = LayerShellQt::Window::get(window);

    // Place the surface on the output the QWindow was assigned to
    // (QWindow::setScreen). Without this LayerShellQt lets the compositor pick
    // the output, so every notch lands on the same (primary) screen and the
    // external monitors get none.
    layer->setScreenConfiguration(LayerShellQt::Window::ScreenFromQWindow);

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
