import QtQuick

// "Liquid glass" styling drawn on top of the (translucent, KWin-blurred) notch:
//   - a soft top sheen,
//   - a slow drifting specular highlight (the "liquid" part),
//   - a bright rim along the edges (fake edge-lensing).
// Real background refraction isn't possible on Wayland (no readback of the
// compositor), so the rim + specular approximate the look.
Item {
    id: glass
    property int bottomRadius: 14

    // Sheen + specular are clipped to the notch bounds.
    Item {
        anchors.fill: parent
        clip: true

        // Top-down sheen.
        Rectangle {
            anchors.fill: parent
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.rgba(1, 1, 1, 0.12) }
                GradientStop { position: 0.45; color: "transparent" }
            }
        }

        // Drifting specular band — slowly sweeps across, then pauses.
        Rectangle {
            id: spec
            width: Math.max(48, glass.width * 0.35)
            height: glass.height * 2.4
            y: -glass.height * 0.7
            rotation: 18
            gradient: Gradient {
                orientation: Gradient.Horizontal
                GradientStop { position: 0.0; color: "transparent" }
                GradientStop { position: 0.5; color: Qt.rgba(1, 1, 1, 0.13) }
                GradientStop { position: 1.0; color: "transparent" }
            }
            SequentialAnimation on x {
                running: glass.visible
                loops: Animation.Infinite
                NumberAnimation {
                    from: -spec.width; to: glass.width
                    duration: 4200; easing.type: Easing.InOutSine
                }
                PauseAnimation { duration: 1800 }
            }
        }
    }

    // Bright rim along the rounded edge.
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        topLeftRadius: 0
        topRightRadius: 0
        bottomLeftRadius: glass.bottomRadius
        bottomRightRadius: glass.bottomRadius
        border.width: 1
        border.color: Qt.rgba(1, 1, 1, 0.20)
    }
}
