#include "MprisPlayer.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusMetaType>
#include <QDBusObjectPath>

namespace {
constexpr auto kObjectPath = "/org/mpris/MediaPlayer2";
constexpr auto kRootIface = "org.mpris.MediaPlayer2";
constexpr auto kPlayerIface = "org.mpris.MediaPlayer2.Player";
constexpr auto kPropsIface = "org.freedesktop.DBus.Properties";

// Read a single property via org.freedesktop.DBus.Properties.Get.
QVariant getProp(QDBusInterface *propsProxy, const QString &iface, const QString &name) {
    QDBusReply<QVariant> reply = propsProxy->call(QStringLiteral("Get"), iface, name);
    return reply.isValid() ? reply.value() : QVariant();
}
} // namespace

MprisPlayer::MprisPlayer(const QString &serviceName, QObject *parent)
    : QObject(parent), m_serviceName(serviceName) {
    auto bus = QDBusConnection::sessionBus();

    m_player = new QDBusInterface(serviceName, kObjectPath, kPlayerIface, bus, this);
    m_root = new QDBusInterface(serviceName, kObjectPath, kRootIface, bus, this);

    // MPRIS players push updates through the standard PropertiesChanged signal.
    bus.connect(serviceName, kObjectPath, kPropsIface,
                QStringLiteral("PropertiesChanged"), this,
                SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));

    refreshAll();
}

qlonglong MprisPlayer::position() const {
    // Called on a timer (~2 Hz) for the seek bar, so build the message directly
    // instead of a QDBusInterface (which would re-introspect the service each call).
    QDBusMessage msg = QDBusMessage::createMethodCall(
        m_serviceName, QLatin1String(kObjectPath), QLatin1String(kPropsIface),
        QStringLiteral("Get"));
    msg << QString::fromLatin1(kPlayerIface) << QStringLiteral("Position");
    QDBusReply<QVariant> reply = QDBusConnection::sessionBus().call(msg);
    return reply.isValid() ? reply.value().toLongLong() : 0;
}

void MprisPlayer::playPause() {
    if (m_player) m_player->asyncCall(QStringLiteral("PlayPause"));
}

void MprisPlayer::next() {
    if (m_player) m_player->asyncCall(QStringLiteral("Next"));
}

void MprisPlayer::previous() {
    if (m_player) m_player->asyncCall(QStringLiteral("Previous"));
}

void MprisPlayer::setPosition(qlonglong micros) {
    if (m_player && !m_trackId.isEmpty()) {
        m_player->asyncCall(QStringLiteral("SetPosition"),
                            QVariant::fromValue(QDBusObjectPath(m_trackId)),
                            QVariant::fromValue(micros));
    }
}

void MprisPlayer::onPropertiesChanged(const QString &interface,
                                      const QVariantMap &changed,
                                      const QStringList &) {
    if (interface != QLatin1String(kPlayerIface)) return;

    if (changed.contains(QStringLiteral("Metadata")))
        applyMetadata(qdbus_cast<QVariantMap>(changed.value(QStringLiteral("Metadata"))));

    if (changed.contains(QStringLiteral("PlaybackStatus"))) {
        m_playbackStatus = changed.value(QStringLiteral("PlaybackStatus")).toString();
        Q_EMIT playbackChanged();
    }
}

void MprisPlayer::refreshAll() {
    auto bus = QDBusConnection::sessionBus();
    QDBusInterface props(m_serviceName, kObjectPath, kPropsIface, bus);

    m_identity = getProp(&props, kRootIface, QStringLiteral("Identity")).toString();
    m_playbackStatus =
        getProp(&props, kPlayerIface, QStringLiteral("PlaybackStatus")).toString();
    applyMetadata(qdbus_cast<QVariantMap>(
        getProp(&props, kPlayerIface, QStringLiteral("Metadata"))));

    Q_EMIT playbackChanged();
}

void MprisPlayer::applyMetadata(const QVariantMap &metadata) {
    m_title = metadata.value(QStringLiteral("xesam:title")).toString();
    m_album = metadata.value(QStringLiteral("xesam:album")).toString();
    m_artUrl = metadata.value(QStringLiteral("mpris:artUrl")).toString();
    m_length = metadata.value(QStringLiteral("mpris:length")).toLongLong();
    m_trackId = metadata.value(QStringLiteral("mpris:trackid")).toString();

    // xesam:artist is a list of strings.
    const QStringList artists =
        metadata.value(QStringLiteral("xesam:artist")).toStringList();
    m_artist = artists.join(QStringLiteral(", "));

    Q_EMIT metadataChanged();
}
