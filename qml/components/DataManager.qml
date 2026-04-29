import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    radius: 8
    color: "#ffffff"
    border.color: "#cfd8e3"

    RowLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 14

        Rectangle {
            Layout.preferredWidth: 42
            Layout.preferredHeight: 42
            radius: 8
            color: "#172033"

            Text {
                anchors.centerIn: parent
                text: "R"
                color: "#ffffff"
                font.pixelSize: 22
                font.bold: true
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                Text {
                    text: "洛克王国：世界 本地图鉴"
                    color: "#111827"
                    font.pixelSize: 22
                    font.bold: true
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                StatusPill {
                    label: dex.dataStatus
                    accent: dex.dataStatus.indexOf("在线") >= 0 ? "#2f7d5c" : "#2f6f8f"
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                MetricText { text: dex.petCount + " 精灵" }
                MetricText { text: dex.skillCount + " 技能" }
                MetricText { text: dex.resultCount + " 当前结果" }
                MetricText { text: "数据时间 " + dex.snapshotTime }

                Text {
                    Layout.fillWidth: true
                    text: dex.updateMessage
                    color: dex.updateMessage.indexOf("失败") >= 0 ? "#b42318" : "#5f6b7a"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                }
            }
        }

        ProgressBar {
            visible: dex.updateBusy
            indeterminate: true
            Layout.preferredWidth: 92
        }

        HeaderButton {
            label: dex.updateBusy ? "检查中" : "检查更新"
            enabled: !dex.updateBusy
            emphasized: true
            onClicked: dex.checkForUpdates()
        }

        HeaderButton {
            label: "恢复内置"
            enabled: !dex.updateBusy
            onClicked: dex.resetToEmbeddedData()
        }
    }

    component MetricText: Text {
        color: "#334155"
        font.pixelSize: 12
        font.bold: true
    }

    component StatusPill: Rectangle {
        property string label: ""
        property string accent: "#2f6f8f"

        Layout.preferredHeight: 28
        Layout.preferredWidth: Math.max(96, statusText.implicitWidth + 22)
        radius: 6
        color: accent === "#2f7d5c" ? "#edf8f2" : "#edf6fb"
        border.color: accent === "#2f7d5c" ? "#cfe9d9" : "#cfe3ec"

        Text {
            id: statusText
            anchors.centerIn: parent
            text: label
            color: accent
            font.pixelSize: 12
            font.bold: true
            elide: Text.ElideRight
            maximumLineCount: 1
        }
    }

    component HeaderButton: Rectangle {
        property string label: ""
        property bool emphasized: false
        signal clicked()

        Layout.preferredWidth: emphasized ? 98 : 92
        Layout.preferredHeight: 38
        radius: 7
        color: !enabled ? "#e6ebf1" : emphasized ? "#2f6f8f" : "#f4f7fa"
        border.color: emphasized ? "#2f6f8f" : "#cfd8e3"

        Text {
            anchors.centerIn: parent
            text: label
            color: !parent.enabled ? "#8a96a6" : parent.emphasized ? "#ffffff" : "#172033"
            font.pixelSize: 13
            font.bold: true
        }

        MouseArea {
            anchors.fill: parent
            enabled: parent.enabled
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: parent.clicked()
        }
    }
}
