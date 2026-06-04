import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

// Standalone settings window (a normal window, not the layer-shell overlay).
// The Appearance page mirrors the macOS original's AppearanceSettingsView,
// grouped into General / Media / Additional / Claude Code.
ApplicationWindow {
    id: win
    title: qsTr("Boring Notch — Settings")
    width: 520
    height: 600
    minimumWidth: 420
    minimumHeight: 420
    // NOTE: do not set `visibility` here — any non-Hidden value would force the
    // window visible at startup. It stays hidden until NotchManager shows it.
    flags: Qt.Dialog

    // Close with Escape (the compositor already provides the window's close
    // button / titlebar).
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
            TabButton { text: qsTr("Appearance") }
            TabButton { text: qsTr("Notch") }
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
                        title: qsTr("General")
                        Layout.topMargin: 18
                        ToggleRow {
                            label: qsTr("Always show tabs")
                            checked: Config.alwaysShowTabs
                            onToggled: value => Config.alwaysShowTabs = value
                        }
                        ToggleRow {
                            label: qsTr("Show settings icon in notch")
                            checked: Config.settingsIconInNotch
                            onToggled: value => Config.settingsIconInNotch = value
                        }
                        ToggleRow {
                            label: qsTr("Show notch on all displays")
                            subtitle: qsTr("Mirror the notch onto every connected monitor.")
                            checked: Config.showOnAllDisplays
                            onToggled: value => Config.showOnAllDisplays = value
                        }
                    }

                    SettingsGroup {
                        title: qsTr("Media")
                        ToggleRow {
                            label: qsTr("Colored spectrogram")
                            checked: Config.coloredSpectrogram
                            onToggled: value => Config.coloredSpectrogram = value
                        }
                        ToggleRow {
                            label: qsTr("Real-time audio waveform")
                            subtitle: qsTr("Uses the playing app's audio via PipeWire. Slightly more CPU. (Visualizer not yet implemented.)")
                            checked: Config.realtimeAudioWaveform
                            onToggled: value => Config.realtimeAudioWaveform = value
                        }
                        ToggleRow {
                            label: qsTr("Player tinting")
                            subtitle: qsTr("Tint the notch with the album art's dominant color.")
                            checked: Config.playerColorTinting
                            onToggled: value => Config.playerColorTinting = value
                        }
                        ToggleRow {
                            label: qsTr("Enable blur effect behind album art")
                            checked: Config.lightingEffect
                            onToggled: value => Config.lightingEffect = value
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            Label { text: qsTr("Slider color"); Layout.fillWidth: true }
                            ComboBox {
                                id: sliderColorBox
                                model: ["Accent", "White", "Album"]
                                currentIndex: Math.max(0, model.indexOf(Config.sliderColor))
                                onActivated: Config.sliderColor = currentText
                            }
                        }
                    }

                    SettingsGroup {
                        title: qsTr("Additional features")
                        ToggleRow {
                            label: qsTr("Show cool face animation while inactive")
                            checked: Config.showFaceAnimation
                            onToggled: value => Config.showFaceAnimation = value
                        }
                    }

                    SettingsGroup {
                        title: qsTr("Claude Code")
                        ToggleRow {
                            label: qsTr("Show Claude Code activity")
                            subtitle: qsTr("Shows in the notch when Claude Code is working, finished, or needs input. Requires the Claude Code status hook.")
                            checked: Config.claudeIndicatorEnabled
                            onToggled: value => Config.claudeIndicatorEnabled = value
                        }
                    }

                    Item { Layout.fillHeight: true; Layout.preferredHeight: 18 }
                }
            }

            // --- Notch (size & shape) page --------------------------------
            ScrollView {
                id: notchScroll
                clip: true
                contentWidth: availableWidth

                ColumnLayout {
                    width: notchScroll.availableWidth
                    spacing: 18

                    SettingsGroup {
                        title: qsTr("Closed (collapsed) notch")
                        Layout.topMargin: 18
                        SpinRow {
                            label: qsTr("Width"); from: 80; to: 1200
                            value: Config.closedNotchWidth
                            onEdited: newValue => Config.closedNotchWidth = newValue
                        }
                        SpinRow {
                            label: qsTr("Height"); from: 12; to: 200
                            value: Config.closedNotchHeight
                            onEdited: newValue => Config.closedNotchHeight = newValue
                        }
                        SpinRow {
                            label: qsTr("Corner radius"); from: 0; to: 60
                            value: Config.closedCornerRadius
                            onEdited: newValue => Config.closedCornerRadius = newValue
                        }
                    }

                    SettingsGroup {
                        title: qsTr("Open (expanded) notch")
                        SpinRow {
                            label: qsTr("Width"); from: 200; to: 1600
                            value: Config.openNotchWidth
                            onEdited: newValue => Config.openNotchWidth = newValue
                        }
                        SpinRow {
                            label: qsTr("Height"); from: 60; to: 800
                            value: Config.openNotchHeight
                            onEdited: newValue => Config.openNotchHeight = newValue
                        }
                        SpinRow {
                            label: qsTr("Corner radius"); from: 0; to: 80
                            value: Config.openCornerRadius
                            onEdited: newValue => Config.openCornerRadius = newValue
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.leftMargin: 18
                        Layout.rightMargin: 18
                        Item { Layout.fillWidth: true }
                        Button {
                            text: qsTr("Reset to defaults")
                            onClicked: Config.resetNotchGeometry()
                        }
                    }

                    Item { Layout.fillHeight: true; Layout.preferredHeight: 18 }
                }
            }
        }
    }

    // --- inline building blocks ---------------------------------------------

    // A titled section. Declared children (the header Label) come first; any
    // rows the caller adds are appended after it in the same ColumnLayout.
    component SettingsGroup: ColumnLayout {
        property string title
        Layout.fillWidth: true
        Layout.leftMargin: 18
        Layout.rightMargin: 18
        spacing: 8
        Label {
            text: title
            font.bold: true
            opacity: 0.7
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
