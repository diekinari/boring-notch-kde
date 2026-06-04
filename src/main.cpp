#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QtQml/qqmlregistration.h>

#include <LayerShellQt/Shell>

#include "AppController.h"
#include "AppSettings.h"
#include "MprisManager.h"
#include "MprisPlayer.h"
#include "NotchWindow.h"

int main(int argc, char *argv[]) {
    // Must be called before the application so Qt's Wayland plugin loads the
    // layer-shell integration.
    LayerShellQt::Shell::useLayerShell();

    // QApplication (not QGuiApplication) because the system-tray icon and its
    // context menu live in QtWidgets.
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("boring-notch-kde"));
    app.setOrganizationName(QStringLiteral("TheBoringTeam"));
    app.setDesktopFileName(QStringLiteral("org.theboringteam.boringnotchkde"));
    // Closing the settings window must not quit the app; only the tray/menu does.
    app.setQuitOnLastWindowClosed(false);

    AppSettings settings;
    AppController controller;
    MprisManager mpris;

    QQmlApplicationEngine engine;
    QQmlContext *ctx = engine.rootContext();
    ctx->setContextProperty(QStringLiteral("Config"), &settings);
    ctx->setContextProperty(QStringLiteral("App"), &controller);
    ctx->setContextProperty(QStringLiteral("Mpris"), &mpris);

    // Promote only the notch (root) window to a layer-shell surface; the
    // settings window is a normal window and must stay untouched.
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [](QObject *obj, const QUrl &) {
            if (auto *win = qobject_cast<QQuickWindow *>(obj)) {
                NotchWindow::configureLayerShell(win);
            }
        });

    engine.loadFromModule(QStringLiteral("BoringNotch"), QStringLiteral("Main"));
    if (engine.rootObjects().isEmpty()) return -1;

    controller.setupTray();

    return app.exec();
}
