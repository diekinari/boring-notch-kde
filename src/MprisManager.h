#pragma once

#include <QObject>
#include <QHash>
#include <QList>
#include <QVariantList>

// Full definition required (not just a forward declaration): MprisManager
// exposes Q_PROPERTY(MprisPlayer *active ...), and Qt6's moc needs the pointed-to
// type to be complete to register its meta type.
#include "MprisPlayer.h"

// Discovers MPRIS players on the session bus, tracks them appearing/vanishing,
// and exposes whichever one is "active" (prefers a Playing player). This is the
// equivalent of the macOS app's MusicManager + MediaController selection logic,
// but driven entirely by D-Bus name ownership instead of polling per-app APIs.
class MprisManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(MprisPlayer *active READ active NOTIFY activeChanged)
    Q_PROPERTY(bool hasPlayer READ hasPlayer NOTIFY activeChanged)
    Q_PROPERTY(int playerCount READ playerCount NOTIFY playersChanged)

public:
    explicit MprisManager(QObject *parent = nullptr);

    MprisPlayer *active() const { return m_active; }
    bool hasPlayer() const { return m_active != nullptr; }
    int playerCount() const { return m_players.size(); }

    // List of {serviceName, identity, isActive} for the player switcher.
    Q_INVOKABLE QVariantList playerInfos() const;
    // Manually make a given player active (and keep it active until it goes away).
    Q_INVOKABLE void activate(const QString &serviceName);

Q_SIGNALS:
    void activeChanged();
    void playersChanged();

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
    QString m_manualService; // user-pinned player, empty = automatic
};
