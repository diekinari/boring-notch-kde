#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QtQml/qqmlregistration.h>

#include <LayerShellQt/Shell>

#include "MprisManager.h"
#include "MprisPlayer.h"
#include "NotchWindow.h"

int main(int argc, char *argv[]) {
    // Must be called before the QGuiApplication so Qt's Wayland plugin loads
    // the layer-shell integration.
    LayerShellQt::Shell::useLayerShell();

    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("boring-notch-kde"));
    app.setOrganizationName(QStringLiteral("TheBoringTeam"));
    app.setDesktopFileName(
        QStringLiteral("org.theboringteam.boringnotchkde"));

    // Expose MPRIS to QML. MprisManager owns the active-player selection; QML
    // binds to manager.active and reacts to activeChanged.
    MprisManager mpris;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("Mpris"), &mpris);

    // When the root QML window is created, promote it to a layer-shell surface
    // before it is shown.
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [](QObject *obj, const QUrl &) {
            if (auto *win = qobject_cast<QQuickWindow *>(obj)) {
                NotchWindow::configureLayerShell(win);
            }
        });

    engine.loadFromModule(QStringLiteral("BoringNotch"), QStringLiteral("Main"));
    if (engine.rootObjects().isEmpty()) return -1;

    return app.exec();
}
