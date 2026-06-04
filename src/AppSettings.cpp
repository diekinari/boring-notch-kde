#include "AppSettings.h"

AppSettings::AppSettings(QObject *parent)
    : QObject(parent),
      m_store(QSettings::IniFormat, QSettings::UserScope,
              QStringLiteral("TheBoringTeam"),
              QStringLiteral("boring-notch-kde")) {
    // Load persisted values, falling back to the in-class defaults.
    m_alwaysShowTabs = m_store.value(QStringLiteral("alwaysShowTabs"), m_alwaysShowTabs).toBool();
    m_settingsIconInNotch = m_store.value(QStringLiteral("settingsIconInNotch"), m_settingsIconInNotch).toBool();
    m_showOnAllDisplays = m_store.value(QStringLiteral("showOnAllDisplays"), m_showOnAllDisplays).toBool();
    m_coloredSpectrogram = m_store.value(QStringLiteral("coloredSpectrogram"), m_coloredSpectrogram).toBool();
    m_realtimeAudioWaveform = m_store.value(QStringLiteral("realtimeAudioWaveform"), m_realtimeAudioWaveform).toBool();
    m_playerColorTinting = m_store.value(QStringLiteral("playerColorTinting"), m_playerColorTinting).toBool();
    m_lightingEffect = m_store.value(QStringLiteral("lightingEffect"), m_lightingEffect).toBool();
    m_sliderColor = m_store.value(QStringLiteral("sliderColor"), m_sliderColor).toString();
    m_showFaceAnimation = m_store.value(QStringLiteral("showFaceAnimation"), m_showFaceAnimation).toBool();
    m_claudeIndicatorEnabled = m_store.value(QStringLiteral("claudeIndicatorEnabled"), m_claudeIndicatorEnabled).toBool();
}

// Each setter is a no-op when the value is unchanged; otherwise it updates the
// member, persists it, and emits the matching change signal so QML re-evaluates.
#define DEFINE_BOOL_SETTER(Setter, Member, Key, Signal)        \
    void AppSettings::Setter(bool v) {                         \
        if (Member == v) return;                               \
        Member = v;                                            \
        store(Key, v);                                         \
        Q_EMIT Signal();                                       \
    }

DEFINE_BOOL_SETTER(setAlwaysShowTabs, m_alwaysShowTabs, "alwaysShowTabs", alwaysShowTabsChanged)
DEFINE_BOOL_SETTER(setSettingsIconInNotch, m_settingsIconInNotch, "settingsIconInNotch", settingsIconInNotchChanged)
DEFINE_BOOL_SETTER(setShowOnAllDisplays, m_showOnAllDisplays, "showOnAllDisplays", showOnAllDisplaysChanged)
DEFINE_BOOL_SETTER(setColoredSpectrogram, m_coloredSpectrogram, "coloredSpectrogram", coloredSpectrogramChanged)
DEFINE_BOOL_SETTER(setRealtimeAudioWaveform, m_realtimeAudioWaveform, "realtimeAudioWaveform", realtimeAudioWaveformChanged)
DEFINE_BOOL_SETTER(setPlayerColorTinting, m_playerColorTinting, "playerColorTinting", playerColorTintingChanged)
DEFINE_BOOL_SETTER(setLightingEffect, m_lightingEffect, "lightingEffect", lightingEffectChanged)
DEFINE_BOOL_SETTER(setShowFaceAnimation, m_showFaceAnimation, "showFaceAnimation", showFaceAnimationChanged)
DEFINE_BOOL_SETTER(setClaudeIndicatorEnabled, m_claudeIndicatorEnabled, "claudeIndicatorEnabled", claudeIndicatorEnabledChanged)

#undef DEFINE_BOOL_SETTER

void AppSettings::setSliderColor(const QString &v) {
    if (m_sliderColor == v) return;
    m_sliderColor = v;
    store("sliderColor", v);
    Q_EMIT sliderColorChanged();
}
