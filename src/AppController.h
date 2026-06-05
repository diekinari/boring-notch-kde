#pragma once

#include <QObject>

class QSystemTrayIcon;
class QMenu;
class QAction;
class AppSettings;

// Owns the system-tray icon (the Linux equivalent of the macOS menu-bar extra)
// and brokers "open settings" / "quit" requests. Exposed to QML as `App`.
//
// Both the tray menu and a right-click on the notch call requestSettings(),
// which emits settingsRequested(); the QML side shows the settings window.
class AppController : public QObject {
    Q_OBJECT
public:
    explicit AppController(AppSettings *settings, QObject *parent = nullptr);

    // Creates the tray icon + context menu. Call once after the app is set up.
    void setupTray();

    Q_INVOKABLE void requestSettings() { Q_EMIT settingsRequested(); }
    Q_INVOKABLE void quit();

    // Open KWin's Desktop Effects settings (where the global Blur strength is).
    Q_INVOKABLE void openBlurSettings();

Q_SIGNALS:
    void settingsRequested();

private Q_SLOTS:
    void retranslate(); // refresh tray menu texts for the current language

private:
    AppSettings *m_settings = nullptr;
    QSystemTrayIcon *m_tray = nullptr;
    QMenu *m_menu = nullptr;
    QAction *m_settingsAction = nullptr;
    QAction *m_quitAction = nullptr;
};
