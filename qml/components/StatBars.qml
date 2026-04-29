import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root
    property var stats: []
    spacing: 10

    Repeater {
        model: root.stats

        delegate: RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Text {
                Layout.preferredWidth: 44
                text: modelData.label
                color: modelData.key === "baseStats" ? "#111827" : "#334155"
                font.pixelSize: 12
                font.bold: modelData.key === "baseStats"
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 10
                radius: 5
                color: "#e6edf3"
                clip: true

                Rectangle {
                    width: parent.width * Math.min(100, modelData.percent) / 100
                    height: parent.height
                    radius: 5
                    color: modelData.key === "baseStats" ? "#2f6f8f" :
                           modelData.key === "speed" ? "#6b8f2f" :
                           modelData.key === "health" ? "#b8543f" : "#526f9f"

                    Behavior on width { NumberAnimation { duration: 180; easing.type: Easing.OutCubic } }
                }
            }

            Text {
                Layout.preferredWidth: 42
                text: modelData.value
                color: "#111827"
                font.pixelSize: 12
                font.bold: true
                horizontalAlignment: Text.AlignRight
            }
        }
    }
}
