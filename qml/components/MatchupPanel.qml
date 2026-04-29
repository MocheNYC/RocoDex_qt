import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: root
    property var matchup
    property bool showNeutral: false
    spacing: 12

    function list(name) {
        if (!matchup || !matchup[name])
            return []
        return matchup[name]
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: 8

        SummaryTile {
            label: "弱点"
            value: root.list("weak").length
            accent: "#b8543f"
            fill: "#fff3ef"
        }

        SummaryTile {
            label: "抵抗"
            value: root.list("resistant").length
            accent: "#2f7d5c"
            fill: "#edf8f2"
        }

        SummaryTile {
            label: "进攻优势"
            value: root.list("offense").length
            accent: "#2f6f8f"
            fill: "#edf6fb"
        }
    }

    MatchGroup {
        title: "防守弱点"
        entries: root.list("weak")
        accent: "#b8543f"
        fill: "#fff7f3"
    }

    MatchGroup {
        title: "防守抵抗"
        entries: root.list("resistant")
        accent: "#2f7d5c"
        fill: "#f0fbf4"
    }

    MatchGroup {
        title: "本属性进攻优势"
        entries: root.list("offense")
        accent: "#2f6f8f"
        fill: "#f0f7fb"
    }

    Rectangle {
        Layout.fillWidth: true
        implicitHeight: neutralColumn.implicitHeight + 18
        radius: 8
        color: "#f7f9fb"
        border.color: "#d9e1ea"

        ColumnLayout {
            id: neutralColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 9
            spacing: 8

            RowLayout {
                Layout.fillWidth: true
                Text {
                    Layout.fillWidth: true
                    text: "普通承伤"
                    color: "#526071"
                    font.pixelSize: 13
                    font.bold: true
                }
                Text {
                    text: root.showNeutral ? "收起" : root.list("neutral").length + " 项"
                    color: "#2f6f8f"
                    font.pixelSize: 12
                    font.bold: true
                }
            }

            Flow {
                Layout.fillWidth: true
                visible: root.showNeutral
                spacing: 6
                Repeater {
                    model: root.list("neutral")
                    delegate: Chip {
                        chipText: modelData.label
                        chipColor: "#ffffff"
                        labelColor: "#526071"
                    }
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: root.showNeutral = !root.showNeutral
        }
    }

    component SummaryTile: Rectangle {
        property string label: ""
        property int value: 0
        property color accent: "#2f6f8f"
        property color fill: "#edf6fb"

        Layout.fillWidth: true
        Layout.preferredHeight: 54
        radius: 8
        color: fill
        border.color: Qt.darker(fill, 1.04)

        Column {
            anchors.centerIn: parent
            spacing: 1

            Text {
                text: value
                color: accent
                font.pixelSize: 18
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: label
                color: "#526071"
                font.pixelSize: 11
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    component Chip: Rectangle {
        property string chipText: ""
        property color chipColor: "#eef2f7"
        property color labelColor: "#334155"
        width: chipLabel.implicitWidth + 16
        height: 27
        radius: 6
        color: chipColor
        border.color: Qt.darker(chipColor, 1.06)

        Text {
            id: chipLabel
            anchors.centerIn: parent
            text: chipText
            color: labelColor
            font.pixelSize: 12
            font.bold: true
            maximumLineCount: 1
        }
    }

    component MatchGroup: Rectangle {
        property string title
        property var entries: []
        property color accent
        property color fill

        Layout.fillWidth: true
        implicitHeight: groupColumn.implicitHeight + 18
        radius: 8
        color: fill
        border.color: Qt.darker(fill, 1.04)

        ColumnLayout {
            id: groupColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 9
            spacing: 8

            RowLayout {
                Layout.fillWidth: true

                Text {
                    text: title
                    color: accent
                    font.pixelSize: 13
                    font.bold: true
                    Layout.fillWidth: true
                }

                Text {
                    text: entries.length + " 项"
                    color: "#697386"
                    font.pixelSize: 12
                }
            }

            Flow {
                Layout.fillWidth: true
                spacing: 6
                Repeater {
                    model: entries
                    delegate: Chip {
                        chipText: modelData.multiplier === 4 ? modelData.label + " x4" :
                                  modelData.multiplier === 2 ? modelData.label + " x2" :
                                  modelData.multiplier === 0.5 ? modelData.label + " x0.5" :
                                  modelData.multiplier === 0.25 ? modelData.label + " x0.25" :
                                  modelData.label
                        chipColor: "#ffffff"
                        labelColor: accent
                    }
                }

                Text {
                    visible: entries.length === 0
                    text: "暂无"
                    color: "#697386"
                    font.pixelSize: 12
                }
            }
        }
    }
}
