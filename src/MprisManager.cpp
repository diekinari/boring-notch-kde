#include "MprisManager.h"
#include "MprisPlayer.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusReply>
#include <QVariantMap>

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
    // The switcher label uses the identity, which may load slightly later.
    connect(player, &MprisPlayer::metadataChanged, this,
            &MprisManager::playersChanged);
    Q_EMIT playersChanged();
}

void MprisManager::removePlayer(const QString &service) {
    if (auto *player = m_players.take(service)) {
        if (m_active == player) m_active = nullptr;
        if (m_manualService == service) m_manualService.clear();
        player->deleteLater();
        Q_EMIT playersChanged();
    }
}

void MprisManager::reevaluateActive() {
    MprisPlayer *best = nullptr;

    // 1) Honour a manual pick while that player still exists.
    if (!m_manualService.isEmpty()) {
        best = m_players.value(m_manualService, nullptr);
        if (!best) m_manualService.clear();
    }
    // 2) Otherwise prefer a currently-playing player, then keep the current one,
    //    then fall back to any available player.
    if (!best)
        for (auto *p : std::as_const(m_players))
            if (p->isPlaying()) { best = p; break; }
    if (!best && m_active && m_players.values().contains(m_active)) best = m_active;
    if (!best && !m_players.isEmpty()) best = *m_players.cbegin();

    if (best != m_active) {
        m_active = best;
        Q_EMIT activeChanged();
    }
}

QVariantList MprisManager::playerInfos() const {
    QVariantList list;
    for (auto it = m_players.cbegin(); it != m_players.cend(); ++it) {
        MprisPlayer *p = it.value();
        QVariantMap m;
        m[QStringLiteral("serviceName")] = it.key();
        m[QStringLiteral("identity")] =
            p->identity().isEmpty() ? it.key() : p->identity();
        m[QStringLiteral("isActive")] = (p == m_active);
        list.append(m);
    }
    return list;
}

void MprisManager::activate(const QString &serviceName) {
    MprisPlayer *p = m_players.value(serviceName, nullptr);
    if (!p) return;
    m_manualService = serviceName;
    if (m_active != p) {
        m_active = p;
        Q_EMIT activeChanged();
    }
}
