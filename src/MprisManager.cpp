#include "MprisManager.h"
#include "MprisPlayer.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusReply>

namespace {
constexpr auto kMprisPrefix = "org.mpris.MediaPlayer2.";
}

MprisManager::MprisManager(QObject *parent) : QObject(parent) {
    auto bus = QDBusConnection::sessionBus();

    // Watch the bus for players coming and going.
    bus.connect(QStringLiteral("org.freedesktop.DBus"),
                QStringLiteral("/org/freedesktop/DBus"),
                QStringLiteral("org.freedesktop.DBus"),
                QStringLiteral("NameOwnerChanged"), this,
                SLOT(onNameOwnerChanged(QString, QString, QString)));

    scanExisting();
}

bool MprisManager::isMprisName(const QString &name) {
    return name.startsWith(QLatin1String(kMprisPrefix));
}

void MprisManager::scanExisting() {
    auto *iface = QDBusConnection::sessionBus().interface();
    const QDBusReply<QStringList> names = iface->registeredServiceNames();
    if (!names.isValid()) return;

    for (const QString &name : names.value()) {
        if (isMprisName(name)) addPlayer(name);
    }
    reevaluateActive();
}

void MprisManager::onNameOwnerChanged(const QString &name, const QString &oldOwner,
                                      const QString &newOwner) {
    if (!isMprisName(name)) return;

    if (oldOwner.isEmpty() && !newOwner.isEmpty()) {
        addPlayer(name);
    } else if (!oldOwner.isEmpty() && newOwner.isEmpty()) {
        removePlayer(name);
    }
    reevaluateActive();
}

void MprisManager::addPlayer(const QString &service) {
    if (m_players.contains(service)) return;
    auto *player = new MprisPlayer(service, this);
    m_players.insert(service, player);
    // A player switching to Playing should be able to steal focus.
    connect(player, &MprisPlayer::playbackChanged, this,
            &MprisManager::reevaluateActive);
}

void MprisManager::removePlayer(const QString &service) {
    if (auto *player = m_players.take(service)) {
        if (m_active == player) m_active = nullptr;
        player->deleteLater();
    }
}

void MprisManager::reevaluateActive() {
    MprisPlayer *best = nullptr;

    // Prefer a currently-playing player; otherwise keep the current one if it
    // still exists; otherwise fall back to any available player.
    for (auto *p : std::as_const(m_players)) {
        if (p->isPlaying()) { best = p; break; }
    }
    if (!best && m_active && m_players.values().contains(m_active)) best = m_active;
    if (!best && !m_players.isEmpty()) best = *m_players.cbegin();

    if (best != m_active) {
        m_active = best;
        Q_EMIT activeChanged();
    }
}
