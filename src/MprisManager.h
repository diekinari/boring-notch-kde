#pragma once

#include <QObject>
#include <QHash>
#include <QList>

class MprisPlayer;

// Discovers MPRIS players on the session bus, tracks them appearing/vanishing,
// and exposes whichever one is "active" (prefers a Playing player). This is the
// equivalent of the macOS app's MusicManager + MediaController selection logic,
// but driven entirely by D-Bus name ownership instead of polling per-app APIs.
class MprisManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(MprisPlayer *active READ active NOTIFY activeChanged)
    Q_PROPERTY(bool hasPlayer READ hasPlayer NOTIFY activeChanged)

public:
    explicit MprisManager(QObject *parent = nullptr);

    MprisPlayer *active() const { return m_active; }
    bool hasPlayer() const { return m_active != nullptr; }

Q_SIGNALS:
    void activeChanged();

private Q_SLOTS:
    void onNameOwnerChanged(const QString &name, const QString &oldOwner,
                            const QString &newOwner);
    void reevaluateActive();

private:
    void scanExisting();
    void addPlayer(const QString &service);
    void removePlayer(const QString &service);
    static bool isMprisName(const QString &name);

    QHash<QString, MprisPlayer *> m_players; // service name -> player
    MprisPlayer *m_active = nullptr;
};
