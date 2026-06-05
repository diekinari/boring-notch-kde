import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import BoringNotch

// Standalone settings window (a normal window, not the layer-shell overlay).
// Strings go through Tr.t(...) so the UI re-translates live when the language
// changes; the current language is pushed into Tr from Config below.
ApplicationWindow {
    id: win
    title: Tr.t("Boring Notch — Settings")
    width: 540
    height: 600
    minimumWidth: 440
    minimumHeight: 420
    flags: Qt.Dialog

    // Drive the translation singleton from the persisted language setting.
    Binding { target: Tr; property: "lang"; value: Config.language }

    // Close with Escape (the compositor provides the titlebar / close button).
    Shortcut {
        sequences: [StandardKey.Close, StandardKey.Cancel]
        onActivated: win.close()
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TabBar {
            id: tabs
            Layout.fillWidth: true
            TabButton { text: Tr.t("Appearance") }
            TabButton { text: Tr.t("Notch") }
            TabButton { text: Tr.t("Glass") }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabs.currentIndex

            // --- Appearance page ------------------------------------------
            ScrollView {
                id: scroll
                clip: true
                contentWidth: availableWidth

                ColumnLayout {
                    width: scroll.availableWidth
                    spacing: 18

                    SettingsGroup {
                        title: Tr.t("General")
                        Layout.topMargin: 18
                        RowLayout {
                            Layout.fillWidth: true
                            Label { text: Tr.t("Language"); Layout.fillWidth: true }
                            ComboBox {
                                model: ["English", "Русский"]
                                currentIndex: Config.language === "ru" ? 1 : 0
                                onActivated: Config.language = (currentIndex === 1 ? "ru" : "en")
                            }
                        }
                        ToggleRow {
                            label: Tr.t("Always show tabs")
                            checked: Config.alwaysShowTabs
                            onToggled: value => Config.alwaysShowTabs = value
                        }
                        ToggleRow {
                            label: Tr.t("Show settings icon in notch")
                            checked: Config.settingsIconInNotch
                            onToggled: value => Config.settingsIconInNotch = value
                        }
                        ToggleRow {
                            label: Tr.t("Show notch on all displays")
                            subtitle: Tr.t("Mirror the notch onto every connected monitor.")
                            checked: Config.showOnAllDisplays
                            onToggled: value => Config.showOnAllDisplays = value
                        }
                    }

                    SettingsGroup {
                        title: Tr.t("Media")
                        ToggleRow {
                            label: Tr.t("Colored spectrogram")
                            checked: Config.coloredSpectrogram
                            onToggled: value => Config.coloredSpectrogram = value
                        }
                        ToggleRow {
                            label: Tr.t("Real-time audio waveform")
                            subtitle: Tr.t("Uses the playing app's audio via PipeWire. Slightly more CPU. (Visualizer not yet implemented.)")
                            checked: Config.realtimeAudioWaveform
                            onToggled: value => Config.realtimeAudioWaveform = value
                        }
                        ToggleRow {
                            label: Tr.t("Player tinting")
                            subtitle: Tr.t("Tint the notch with the album art's dominant color.")
                            checked: Config.playerColorTinting
                            onToggled: value => Config.playerColorTinting = value
                        }
                        ToggleRow {
                            label: Tr.t("Enable blur effect behind album art")
                            checked: Config.lightingEffect
                            onToggled: value => Config.lightingEffect = value
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            Label { text: Tr.t("Slider color"); Layout.fillWidth: true }
                            ComboBox {
                                model: ["Accent", "White", "Album"]
                                currentIndex: Math.max(0, model.indexOf(Config.sliderColor))
                                onActivated: Config.sliderColor = currentText
                            }
                        }
                    }

                    SettingsGroup {
                        title: Tr.t("Additional features")
                        ToggleRow {
                            label: Tr.t("Show cool face animation while inactive")
                            checked: Config.showFaceAnimation
                            onToggled: value => Config.showFaceAnimation = value
                        }
                    }

                    SettingsGroup {
                        title: "Claude Code"
                        ToggleRow {
                            label: Tr.t("Show Claude Code activity")
                            subtitle: Tr.t("Shows in the notch when Claude Code is working, finished, or needs input. Requires the Claude Code status hook.")
                            checked: Config.claudeIndicatorEnabled
                            onToggled: value => Config.claudeIndicatorEnabled = value
                        }
                    }

                    Item { Layout.fillHeight: true; Layout.preferredHeight: 18 }
                }
            }

            // --- Notch page ----------------------------------------------
            ScrollView {
                id: notchScroll
                clip: true
                contentWidth: availableWidth

                ColumnLayout {
                    width: notchScroll.availableWidth
                    spacing: 18

                    SettingsGroup {
                        title: Tr.t("Closed (collapsed) notch")
                        Layout.topMargin: 18
                        SpinRow {
                            label: Tr.t("Width"); from: 80; to: 1200
                            value: Config.closedNotchWidth
                            onEdited: newValue => Config.closedNotchWidth = newValue
                        }
                        SpinRow {
                            label: Tr.t("Height"); from: 12; to: 200
                            value: Config.closedNotchHeight
                            onEdited: newValue => Config.closedNotchHeight = newValue
                        }
                        SpinRow {
                            label: Tr.t("Corner radius"); from: 0; to: 60
                            value: Config.closedCornerRadius
                            onEdited: newValue => Config.closedCornerRadius = newValue
                        }
                    }

                    SettingsGroup {
                        title: Tr.t("Open (expanded) notch")
                        SpinRow {
                            label: Tr.t("Width"); from: 200; to: 1600
                            value: Config.openNotchWidth
                            onEdited: newValue => Config.openNotchWidth = newValue
                        }
                        SpinRow {
                            label: Tr.t("Height"); from: 60; to: 800
                            value: Config.openNotchHeight
                            onEdited: newValue => Config.openNotchHeight = newValue
                        }
                        SpinRow {
                            label: Tr.t("Corner radius"); from: 0; to: 80
                            value: Config.openCornerRadius
                            onEdited: newValue => Config.openCornerRadius = newValue
                        }
                    }

                    ResetRow { onReset: Config.resetNotchGeometry() }

                    Item { Layout.fillHeight: true; Layout.preferredHeight: 18 }
                }
            }

            // --- Glass page ----------------------------------------------
            ScrollView {
                id: glassScroll
                clip: true
                contentWidth: availableWidth

                ColumnLayout {
                    width: glassScroll.availableWidth
                    spacing: 18

                    SettingsGroup {
                        title: Tr.t("Liquid glass")
                        Layout.topMargin: 18
                        ToggleRow {
                            label: Tr.t("Enable liquid glass")
                            checked: Config.liquidGlass
                            onToggled: value => Config.liquidGlass = value
                        }
                        ToggleRow {
                            label: Tr.t("Background blur (KWin)")
                            subtitle: Tr.t("Frosted desktop behind the notch. Needs KWin's Blur desktop effect enabled.")
                            checked: Config.glassBlur
                            onToggled: value => Config.glassBlur = value
                        }
                        SpinRow {
                            label: Tr.t("Opacity (%)"); from: 0; to: 100
                            value: Config.glassOpacity
                            onEdited: newValue => Config.glassOpacity = newValue
                        }
                        SpinRow {
                            label: Tr.t("Sheen (%)"); from: 0; to: 100
                            value: Config.glassSheen
                            onEdited: newValue => Config.glassSheen = newValue
                        }
                        SpinRow {
                            label: Tr.t("Rim highlight (%)"); from: 0; to: 100
                            value: Config.glassRim
                            onEdited: newValue => Config.glassRim = newValue
                        }
                    }

                    ResetRow { onReset: Config.resetGlass() }

                    Item { Layout.fillHeight: true; Layout.preferredHeight: 18 }
                }
            }
        }
    }

    // --- inline building blocks ---------------------------------------------

    // A titled section. Declared children (the header Label) come first; any
    // rows the caller adds are appended after it in the same ColumnLayout.
    component SettingsGroup: ColumnLayout {
        id: grp
        property string title
        Layout.fillWidth: true
        Layout.leftMargin: 18
        Layout.rightMargin: 18
        spacing: 8
        Label {
            text: grp.title
            font.bold: true
            opacity: 0.7
        }
    }

    // A right-aligned "Reset to defaults" button row.
    component ResetRow: RowLayout {
        id: rrow
        signal reset()
        Layout.fillWidth: true
        Layout.leftMargin: 18
        Layout.rightMargin: 18
        Item { Layout.fillWidth: true }
        Button {
            text: Tr.t("Reset to defaults")
            onClicked: rrow.reset()
        }
    }

    // A label (+ optional subtitle) with a trailing Switch. Two-way safe: the
    // Switch is bound to `checked`; on user toggle we restore the binding and
    // emit toggled(value), so it stays in sync with Config even after clicks.
    component ToggleRow: RowLayout {
        id: row
        property string label
        property string subtitle
        property bool checked: false
        signal toggled(bool value)

        Layout.fillWidth: true
        spacing: 12

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2
            Label { text: row.label; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            Label {
                text: row.subtitle
                visible: row.subtitle.length > 0
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                font.pixelSize: 11
                opacity: 0.55
            }
        }
        Switch {
            id: sw
            checked: row.checked
            onToggled: {
                const v = checked;
                checked = Qt.binding(function () { return row.checked; });
                row.toggled(v);
            }
        }
    }

    // A label + integer SpinBox. Same two-way-safe pattern as ToggleRow.
    component SpinRow: RowLayout {
        id: srow
        property string label
        property int from: 0
        property int to: 2000
        property int step: 1
        property int value: 0
        signal edited(int newValue)

        Layout.fillWidth: true
        spacing: 12

        Label { text: srow.label; Layout.fillWidth: true }
        SpinBox {
            from: srow.from
            to: srow.to
            stepSize: srow.step
            editable: true
            value: srow.value
            onValueModified: {
                const v = value;
                value = Qt.binding(function () { return srow.value; });
                srow.edited(v);
            }
        }
    }
}
