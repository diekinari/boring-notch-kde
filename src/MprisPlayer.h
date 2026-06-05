#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>

class QDBusInterface;

// Wraps a single MPRIS2 player exposed on the session bus under the well-known
// name org.mpris.MediaPlayer2.<something> (e.g. spotify, firefox.instance123).
//
// MPRIS2 is the freedesktop standard every Linux media player speaks, so this
// one class replaces the macOS app's per-app controllers (Spotify/AppleMusic/
// NowPlaying/YouTubeMusic). See https://specifications.freedesktop.org/mpris-spec
class MprisPlayer : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString serviceName READ serviceName CONSTANT)
    Q_PROPERTY(QString identity READ identity NOTIFY metadataChanged)
    Q_PROPERTY(QString title READ title NOTIFY metadataChanged)
    Q_PROPERTY(QString artist READ artist NOTIFY metadataChanged)
    Q_PROPERTY(QString album READ album NOTIFY metadataChanged)
    Q_PROPERTY(QString artUrl READ artUrl NOTIFY metadataChanged)
    Q_PROPERTY(qlonglong length READ length NOTIFY metadataChanged)   // microseconds
    Q_PROPERTY(QString playbackStatus READ playbackStatus NOTIFY playbackChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY playbackChanged)

public:
    explicit MprisPlayer(const QString &serviceName, QObject *parent = nullptr);

    QString serviceName() const { return m_serviceName; }
    QString identity() const { return m_identity; }
    QString title() const { return m_title; }
    QString artist() const { return m_artist; }
    QString album() const { return m_album; }
    QString artUrl() const { return m_artUrl; }
    qlonglong length() const { return m_length; }
    QString playbackStatus() const { return m_playbackStatus; }
    bool isPlaying() const { return m_playbackStatus == QLatin1String("Playing"); }

    // Live playback position in microseconds (queried on demand; MPRIS has no
    // position-changed signal beyond the rarely-emitted Seeked).
    Q_INVOKABLE qlonglong position() const;

    // Monotonic "last became active/playing" stamp, set by MprisManager and used
    // to break ties when picking which player to show.
    quint64 activitySeq() const { return m_activitySeq; }
    void setActivitySeq(quint64 seq) { m_activitySeq = seq; }

public Q_SLOTS:
    void playPause();
    void next();
    void previous();
    void setPosition(qlonglong micros); // absolute seek

Q_SIGNALS:
    void metadataChanged();
    void playbackChanged();

private Q_SLOTS:
    void onPropertiesChanged(const QString &interface,
                             const QVariantMap &changed,
                             const QStringList &invalidated);

private:
    void refreshAll();
    void applyMetadata(const QVariantMap &metadata);

    QString m_serviceName;
    QString m_identity;
    QString m_title;
    QString m_artist;
    QString m_album;
    QString m_artUrl;
    qlonglong m_length = 0;
    QString m_trackId; // mpris:trackid, needed for SetPosition
    QString m_playbackStatus = QStringLiteral("Stopped");
    quint64 m_activitySeq = 0;

    QDBusInterface *m_player = nullptr; // org.mpris.MediaPlayer2.Player
    QDBusInterface *m_root = nullptr;   // org.mpris.MediaPlayer2 (Identity)
};
