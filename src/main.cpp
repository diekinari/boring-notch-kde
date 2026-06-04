#include <QApplication>
#include <QQmlContext>
#include <QQmlEngine>

#include "AppController.h"
#include "AppSettings.h"
#include "MprisManager.h"
#include "MprisPlayer.h"
#include "NotchManager.h"

int main(int argc, char *argv[]) {
    // Note: LayerShellQt::Shell::useLayerShell() is intentionally not called —
    // it's a no-op (and deprecated) since Qt 6.5, which is our minimum. The
    // layer-shell integration is applied per-window in NotchWindow.

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

    QQmlEngine engine;
    QQmlContext *ctx = engine.rootContext();
    ctx->setContextProperty(QStringLiteral("Config"), &settings);
    ctx->setContextProperty(QStringLiteral("App"), &controller);
    ctx->setContextProperty(QStringLiteral("Mpris"), &mpris);

    // NotchManager creates one layer-shell notch per screen plus the settings
    // window, and rebuilds when monitors change or the option toggles.
    NotchManager notches(&engine, &settings);
    QObject::connect(&controller, &AppController::settingsRequested, &notches,
                     &NotchManager::showSettings);
    notches.start();

    controller.setupTray();

    return app.exec();
}
