#pragma once

#include <QHash>
#include <QObject>
#include <QPointer>

class QQmlEngine;
class QQuickWindow;
class QScreen;
class AppSettings;

// Owns the per-screen notch windows and the single settings window.
//
// The macOS original shows the notch on the built-in display and, when
// `showOnAllDisplays` is on, mirrors it onto external monitors. On Wayland each
// monitor needs its own layer-shell surface bound to that output, so we create
// one Notch window per target screen and assign it via QWindow::setScreen()
// before configuring the layer surface.
//
// Rebuilds reactively when monitors are plugged/unplugged or the option toggles.
class NotchManager : public QObject {
    Q_OBJECT
public:
    NotchManager(QQmlEngine *engine, AppSettings *settings,
                 QObject *parent = nullptr);

    // Create the settings window (hidden) and the initial set of notches.
    void start();

public Q_SLOTS:
    void showSettings();

private Q_SLOTS:
    void rebuildNotches();
    void applyGlass();

private:
    QList<QScreen *> targetScreens() const;
    QQuickWindow *createNotch(QScreen *screen);
    void applyGlassTo(QQuickWindow *win);

    QQmlEngine *m_engine = nullptr;
    AppSettings *m_settings = nullptr;
    QPointer<QQuickWindow> m_settingsWindow;
    QHash<QScreen *, QQuickWindow *> m_notches; // screen -> its notch window
};
