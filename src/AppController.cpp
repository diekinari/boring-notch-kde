#include "AppController.h"
#include "AppSettings.h"

#include <QAction>
#include <QApplication>
#include <QIcon>
#include <QMenu>
#include <QProcess>
#include <QSystemTrayIcon>

AppController::AppController(AppSettings *settings, QObject *parent)
    : QObject(parent), m_settings(settings) {}

void AppController::setupTray() {
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        // No StatusNotifier host (rare on KDE). Settings stay reachable via
        // right-clicking the notch, so this isn't fatal.
        return;
    }

    m_menu = new QMenu();
    m_settingsAction = m_menu->addAction(QString());
    connect(m_settingsAction, &QAction::triggered, this,
            &AppController::requestSettings);
    m_menu->addSeparator();
    m_quitAction = m_menu->addAction(QString());
    connect(m_quitAction, &QAction::triggered, this, &AppController::quit);

    retranslate();
    if (m_settings)
        connect(m_settings, &AppSettings::languageChanged, this,
                &AppController::retranslate);

    m_tray = new QSystemTrayIcon(this);
    // Prefer the bundled icon; fall back to a generic theme icon.
    QIcon icon = QIcon::fromTheme(QStringLiteral("boring-notch-kde"),
                                  QIcon::fromTheme(QStringLiteral("applications-multimedia")));
    m_tray->setIcon(icon);
    m_tray->setToolTip(QStringLiteral("Boring Notch"));
    m_tray->setContextMenu(m_menu);

    // Intentionally no left-click action: the right-click menu already offers
    // Settings/Quit, and a single click shouldn't pop the settings window open.

    m_tray->show();
}

void AppController::quit() {
    qApp->quit();
}

void AppController::openBlurSettings() {
    // The KWin Blur effect (with its global strength slider) lives in the
    // Desktop Effects KCM. Try kcmshell6, then fall back to systemsettings.
    if (QProcess::startDetached(QStringLiteral("kcmshell6"),
                                {QStringLiteral("kcm_kwin_effects")}))
        return;
    QProcess::startDetached(QStringLiteral("systemsettings"),
                            {QStringLiteral("kcm_kwin_effects")});
}

void AppController::retranslate() {
    const bool ru = m_settings && m_settings->language() == QLatin1String("ru");
    if (m_settingsAction)
        m_settingsAction->setText(ru ? QStringLiteral("Настройки…")
                                     : QStringLiteral("Settings…"));
    if (m_quitAction)
        m_quitAction->setText(ru ? QStringLiteral("Выход")
                                 : QStringLiteral("Quit"));
}
