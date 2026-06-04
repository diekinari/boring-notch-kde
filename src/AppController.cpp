#include "AppController.h"

#include <QAction>
#include <QApplication>
#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>

AppController::AppController(QObject *parent) : QObject(parent) {}

void AppController::setupTray() {
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        // No StatusNotifier host (rare on KDE). Settings stay reachable via
        // right-clicking the notch, so this isn't fatal.
        return;
    }

    m_menu = new QMenu();
    QAction *settingsAction = m_menu->addAction(tr("Settings…"));
    connect(settingsAction, &QAction::triggered, this,
            &AppController::requestSettings);
    m_menu->addSeparator();
    QAction *quitAction = m_menu->addAction(tr("Quit"));
    connect(quitAction, &QAction::triggered, this, &AppController::quit);

    m_tray = new QSystemTrayIcon(this);
    // Prefer the bundled icon; fall back to a generic theme icon.
    QIcon icon = QIcon::fromTheme(QStringLiteral("boring-notch-kde"),
                                  QIcon::fromTheme(QStringLiteral("applications-multimedia")));
    m_tray->setIcon(icon);
    m_tray->setToolTip(QStringLiteral("Boring Notch"));
    m_tray->setContextMenu(m_menu);

    // Left-click also opens settings, matching common tray conventions.
    connect(m_tray, &QSystemTrayIcon::activated, this,
            [this](QSystemTrayIcon::ActivationReason reason) {
                if (reason == QSystemTrayIcon::Trigger) requestSettings();
            });

    m_tray->show();
}

void AppController::quit() {
    qApp->quit();
}
