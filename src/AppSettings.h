#pragma once

#include <QObject>
#include <QSettings>
#include <QString>

// Persistent appearance/behaviour settings, mirroring the macOS original's
// Appearance pane (boring.notch AppearanceSettingsView). Backed by QSettings, so
// values live in ~/.config/TheBoringTeam/boring-notch-kde.conf (XDG standard).
//
// Exposed to QML as the context property `Config`. Each property has a NOTIFY
// signal, so the notch UI binds to it and updates live as the user toggles
// things in the settings window.
class AppSettings : public QObject {
    Q_OBJECT
    // --- General ---
    Q_PROPERTY(bool alwaysShowTabs READ alwaysShowTabs WRITE setAlwaysShowTabs NOTIFY alwaysShowTabsChanged)
    Q_PROPERTY(bool settingsIconInNotch READ settingsIconInNotch WRITE setSettingsIconInNotch NOTIFY settingsIconInNotchChanged)
    // --- Media ---
    Q_PROPERTY(bool coloredSpectrogram READ coloredSpectrogram WRITE setColoredSpectrogram NOTIFY coloredSpectrogramChanged)
    Q_PROPERTY(bool realtimeAudioWaveform READ realtimeAudioWaveform WRITE setRealtimeAudioWaveform NOTIFY realtimeAudioWaveformChanged)
    Q_PROPERTY(bool playerColorTinting READ playerColorTinting WRITE setPlayerColorTinting NOTIFY playerColorTintingChanged)
    Q_PROPERTY(bool lightingEffect READ lightingEffect WRITE setLightingEffect NOTIFY lightingEffectChanged)
    Q_PROPERTY(QString sliderColor READ sliderColor WRITE setSliderColor NOTIFY sliderColorChanged)
    // --- Additional ---
    Q_PROPERTY(bool showFaceAnimation READ showFaceAnimation WRITE setShowFaceAnimation NOTIFY showFaceAnimationChanged)
    // --- Claude Code ---
    Q_PROPERTY(bool claudeIndicatorEnabled READ claudeIndicatorEnabled WRITE setClaudeIndicatorEnabled NOTIFY claudeIndicatorEnabledChanged)

public:
    explicit AppSettings(QObject *parent = nullptr);

    bool alwaysShowTabs() const { return m_alwaysShowTabs; }
    bool settingsIconInNotch() const { return m_settingsIconInNotch; }
    bool coloredSpectrogram() const { return m_coloredSpectrogram; }
    bool realtimeAudioWaveform() const { return m_realtimeAudioWaveform; }
    bool playerColorTinting() const { return m_playerColorTinting; }
    bool lightingEffect() const { return m_lightingEffect; }
    QString sliderColor() const { return m_sliderColor; }
    bool showFaceAnimation() const { return m_showFaceAnimation; }
    bool claudeIndicatorEnabled() const { return m_claudeIndicatorEnabled; }

    void setAlwaysShowTabs(bool v);
    void setSettingsIconInNotch(bool v);
    void setColoredSpectrogram(bool v);
    void setRealtimeAudioWaveform(bool v);
    void setPlayerColorTinting(bool v);
    void setLightingEffect(bool v);
    void setSliderColor(const QString &v);
    void setShowFaceAnimation(bool v);
    void setClaudeIndicatorEnabled(bool v);

Q_SIGNALS:
    void alwaysShowTabsChanged();
    void settingsIconInNotchChanged();
    void coloredSpectrogramChanged();
    void realtimeAudioWaveformChanged();
    void playerColorTintingChanged();
    void lightingEffectChanged();
    void sliderColorChanged();
    void showFaceAnimationChanged();
    void claudeIndicatorEnabledChanged();

private:
    template <class T>
    void store(const char *key, const T &value) {
        m_store.setValue(QString::fromLatin1(key), value);
    }

    QSettings m_store;

    bool m_alwaysShowTabs = false;
    bool m_settingsIconInNotch = true;
    bool m_coloredSpectrogram = true;
    bool m_realtimeAudioWaveform = false;
    bool m_playerColorTinting = true;
    bool m_lightingEffect = true;
    QString m_sliderColor = QStringLiteral("Accent");
    bool m_showFaceAnimation = false;
    bool m_claudeIndicatorEnabled = false;
};
