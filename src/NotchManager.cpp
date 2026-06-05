#include "NotchManager.h"
#include "AppSettings.h"
#include "NotchWindow.h"

#include <QGuiApplication>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QScreen>

NotchManager::NotchManager(QQmlEngine *engine, AppSettings *settings,
                           QObject *parent)
    : QObject(parent), m_engine(engine), m_settings(settings) {}

void NotchManager::start() {
    // Single, reusable settings window (created hidden; QML sets visible:false).
    QQmlComponent settingsComp(m_engine);
    settingsComp.loadFromModule("BoringNotch", "SettingsWindow");
    if (settingsComp.isError()) {
        qWarning() << "Failed to load SettingsWindow:" << settingsComp.errors();
    } else {
        QObject *obj = settingsComp.create(m_engine->rootContext());
        m_settingsWindow = qobject_cast<QQuickWindow *>(obj);
        // QObject:: qualifier is required: QWindow::setParent(QWindow*) hides
        // QObject::setParent(QObject*), so an unqualified call wouldn't compile.
        if (m_settingsWindow) m_settingsWindow->QObject::setParent(this);
    }

    // Rebuild when monitors are added/removed or the primary screen changes.
    connect(qApp, &QGuiApplication::screenAdded, this,
            &NotchManager::rebuildNotches);
    connect(qApp, &QGuiApplication::screenRemoved, this,
            &NotchManager::rebuildNotches);
    connect(qApp, &QGuiApplication::primaryScreenChanged, this,
            &NotchManager::rebuildNotches);
    // ...and when the user toggles "show on all displays".
    connect(m_settings, &AppSettings::showOnAllDisplaysChanged, this,
            &NotchManager::rebuildNotches);
    // Re-apply blur / frost when any of the glass options change.
    connect(m_settings, &AppSettings::liquidGlassChanged, this,
            &NotchManager::applyGlass);
    connect(m_settings, &AppSettings::glassBlurChanged, this,
            &NotchManager::applyGlass);
    connect(m_settings, &AppSettings::glassFrostChanged, this,
            &NotchManager::applyGlass);

    rebuildNotches();
}

void NotchManager::applyGlass() {
    for (QQuickWindow *win : std::as_const(m_notches)) applyGlassTo(win);
}

void NotchManager::applyGlassTo(QQuickWindow *win) {
    const bool glass = m_settings->liquidGlass();
    NotchWindow::setGlass(win, glass && m_settings->glassBlur());

    // Map the 0..100 "frost" strength onto KWin background-contrast params:
    // darker and more desaturated as it grows. 0 disables the effect.
    const qreal frost = glass ? m_settings->glassFrost() / 100.0 : 0.0;
    NotchWindow::setFrost(win, frost > 0.0,
                          /*contrast*/ 1.0,
                          /*intensity*/ 1.0 - 0.4 * frost,
                          /*saturation*/ 1.0 - 0.6 * frost);
}

QList<QScreen *> NotchManager::targetScreens() const {
    if (m_settings->showOnAllDisplays()) return qApp->screens();
    if (QScreen *primary = qApp->primaryScreen()) return {primary};
    return {};
}

QQuickWindow *NotchManager::createNotch(QScreen *screen) {
    QQmlComponent comp(m_engine);
    comp.loadFromModule("BoringNotch", "Notch");
    if (comp.isError()) {
        qWarning() << "Failed to load Notch:" << comp.errors();
        return nullptr;
    }

    QObject *obj = comp.create(m_engine->rootContext());
    auto *win = qobject_cast<QQuickWindow *>(obj);
    if (!win) {
        delete obj;
        return nullptr;
    }
    win->QObject::setParent(this); // see note above re: QWindow::setParent hiding

    // Bind to the target output, then promote to a layer-shell surface, both
    // before the window is shown.
    win->setScreen(screen);
    NotchWindow::configureLayerShell(win, screen);
    applyGlassTo(win);
    win->setVisible(true);
    qInfo() << "[notch] created notch on screen" << screen->name()
            << "-> window now on" << (win->screen() ? win->screen()->name() : "<null>");
    return win;
}

void NotchManager::rebuildNotches() {
    const QList<QScreen *> targets = targetScreens();

    qInfo() << "[notch] rebuild:" << qApp->screens().size()
            << "screen(s) total," << targets.size() << "target(s);"
            << "showOnAllDisplays =" << m_settings->showOnAllDisplays();
    for (QScreen *s : targets)
        qInfo() << "[notch]   target screen:" << s->name() << s->geometry();

    // Drop notches whose screen is gone or no longer targeted.
    const auto current = m_notches.keys();
    for (QScreen *screen : current) {
        if (!targets.contains(screen)) {
            if (QQuickWindow *win = m_notches.take(screen)) win->deleteLater();
        }
    }

    // Add notches for newly targeted screens.
    for (QScreen *screen : targets) {
        if (!m_notches.contains(screen)) {
            if (QQuickWindow *win = createNotch(screen))
                m_notches.insert(screen, win);
        }
    }
}

void NotchManager::showSettings() {
    if (!m_settingsWindow) return;
    m_settingsWindow->show();
    m_settingsWindow->raise();
    m_settingsWindow->requestActivate();
}
