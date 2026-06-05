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
    Q_PROPERTY(bool showOnAllDisplays READ showOnAllDisplays WRITE setShowOnAllDisplays NOTIFY showOnAllDisplaysChanged)
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
    // --- Glass ---
    Q_PROPERTY(bool liquidGlass READ liquidGlass WRITE setLiquidGlass NOTIFY liquidGlassChanged)
    // --- Notch geometry (pixels) ---
    Q_PROPERTY(int closedNotchWidth READ closedNotchWidth WRITE setClosedNotchWidth NOTIFY closedNotchWidthChanged)
    Q_PROPERTY(int closedNotchHeight READ closedNotchHeight WRITE setClosedNotchHeight NOTIFY closedNotchHeightChanged)
    Q_PROPERTY(int openNotchWidth READ openNotchWidth WRITE setOpenNotchWidth NOTIFY openNotchWidthChanged)
    Q_PROPERTY(int openNotchHeight READ openNotchHeight WRITE setOpenNotchHeight NOTIFY openNotchHeightChanged)
    Q_PROPERTY(int closedCornerRadius READ closedCornerRadius WRITE setClosedCornerRadius NOTIFY closedCornerRadiusChanged)
    Q_PROPERTY(int openCornerRadius READ openCornerRadius WRITE setOpenCornerRadius NOTIFY openCornerRadiusChanged)

public:
    explicit AppSettings(QObject *parent = nullptr);

    bool alwaysShowTabs() const { return m_alwaysShowTabs; }
    bool settingsIconInNotch() const { return m_settingsIconInNotch; }
    bool showOnAllDisplays() const { return m_showOnAllDisplays; }
    bool coloredSpectrogram() const { return m_coloredSpectrogram; }
    bool realtimeAudioWaveform() const { return m_realtimeAudioWaveform; }
    bool playerColorTinting() const { return m_playerColorTinting; }
    bool lightingEffect() const { return m_lightingEffect; }
    QString sliderColor() const { return m_sliderColor; }
    bool showFaceAnimation() const { return m_showFaceAnimation; }
    bool claudeIndicatorEnabled() const { return m_claudeIndicatorEnabled; }
    bool liquidGlass() const { return m_liquidGlass; }
    int closedNotchWidth() const { return m_closedNotchWidth; }
    int closedNotchHeight() const { return m_closedNotchHeight; }
    int openNotchWidth() const { return m_openNotchWidth; }
    int openNotchHeight() const { return m_openNotchHeight; }
    int closedCornerRadius() const { return m_closedCornerRadius; }
    int openCornerRadius() const { return m_openCornerRadius; }

    void setAlwaysShowTabs(bool v);
    void setSettingsIconInNotch(bool v);
    void setShowOnAllDisplays(bool v);
    void setColoredSpectrogram(bool v);
    void setRealtimeAudioWaveform(bool v);
    void setPlayerColorTinting(bool v);
    void setLightingEffect(bool v);
    void setSliderColor(const QString &v);
    void setShowFaceAnimation(bool v);
    void setClaudeIndicatorEnabled(bool v);
    void setLiquidGlass(bool v);
    void setClosedNotchWidth(int v);
    void setClosedNotchHeight(int v);
    void setOpenNotchWidth(int v);
    void setOpenNotchHeight(int v);
    void setClosedCornerRadius(int v);
    void setOpenCornerRadius(int v);

    // Restore the notch size/radius to the built-in defaults (macOS-like).
    Q_INVOKABLE void resetNotchGeometry();

Q_SIGNALS:
    void alwaysShowTabsChanged();
    void settingsIconInNotchChanged();
    void showOnAllDisplaysChanged();
    void coloredSpectrogramChanged();
    void realtimeAudioWaveformChanged();
    void playerColorTintingChanged();
    void lightingEffectChanged();
    void sliderColorChanged();
    void showFaceAnimationChanged();
    void claudeIndicatorEnabledChanged();
    void liquidGlassChanged();
    void closedNotchWidthChanged();
    void closedNotchHeightChanged();
    void openNotchWidthChanged();
    void openNotchHeightChanged();
    void closedCornerRadiusChanged();
    void openCornerRadiusChanged();

private:
    template <class T>
    void store(const char *key, const T &value) {
        m_store.setValue(QString::fromLatin1(key), value);
    }

    QSettings m_store;

    bool m_alwaysShowTabs = false;
    bool m_settingsIconInNotch = true;
    bool m_showOnAllDisplays = true; // notch on every monitor by default
    bool m_coloredSpectrogram = true;
    bool m_realtimeAudioWaveform = false;
    bool m_playerColorTinting = true;
    bool m_lightingEffect = true;
    QString m_sliderColor = QStringLiteral("Accent");
    bool m_showFaceAnimation = false;
    bool m_claudeIndicatorEnabled = false;
    bool m_liquidGlass = false;

    // Defaults mirror the macOS original (see Notch.qml comments).
    static constexpr int kDefClosedW = 185;
    static constexpr int kDefClosedH = 32;
    static constexpr int kDefOpenW = 640;
    static constexpr int kDefOpenH = 190;
    static constexpr int kDefClosedR = 14;
    static constexpr int kDefOpenR = 24;

    int m_closedNotchWidth = kDefClosedW;
    int m_closedNotchHeight = kDefClosedH;
    int m_openNotchWidth = kDefOpenW;
    int m_openNotchHeight = kDefOpenH;
    int m_closedCornerRadius = kDefClosedR;
    int m_openCornerRadius = kDefOpenR;
};
