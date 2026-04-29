import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    radius: 8
    color: "#ffffff"
    border.color: "#cfd8e3"

    function sortIndex() {
        for (var i = 0; i < sortModel.length; i++) {
            if (sortModel[i].key === dex.sortKey)
                return i
        }
        return 0
    }

    readonly property var sortModel: [
        { key: "id", label: "编号" },
        { key: "baseStats", label: "种族值总和" },
        { key: "speed", label: "速度" },
        { key: "health", label: "精力" },
        { key: "name", label: "名称" }
    ]

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 14

        RowLayout {
            Layout.fillWidth: true

            ColumnLayout {
                spacing: 2
                Layout.fillWidth: true

                Text {
                    text: "检索与筛选"
                    color: "#111827"
                    font.pixelSize: 18
                    font.bold: true
                }

                Text {
                    text: "支持编号、名称、首字母、分布、技能"
                    color: "#697386"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }

            Rectangle {
                Layout.preferredWidth: 38
                Layout.preferredHeight: 30
                radius: 6
                color: "#edf4f7"

                Text {
                    anchors.centerIn: parent
                    text: dex.resultCount
                    color: "#2f6f8f"
                    font.pixelSize: 13
                    font.bold: true
                }
            }
        }

        TextField {
            id: search
            Layout.fillWidth: true
            Layout.preferredHeight: 42
            text: dex.query
            placeholderText: "搜索精灵或技能"
            selectByMouse: true
            leftPadding: 36
            rightPadding: 10
            color: "#111827"
            placeholderTextColor: "#8793a3"
            onTextEdited: dex.query = text

            background: Rectangle {
                radius: 7
                color: "#f7f9fb"
                border.color: search.activeFocus ? "#2f6f8f" : "#d9e1ea"
            }

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 12
                anchors.verticalCenter: parent.verticalCenter
                text: "⌕"
                color: "#697386"
                font.pixelSize: 18
            }
        }

        Separator {}

        SectionTitle { label: "属性" }

        Flow {
            Layout.fillWidth: true
            spacing: 6
            Repeater {
                model: dex.attributes
                delegate: AttributeBadge {
                    label: modelData.label
                    badgeColor: modelData.color
                    textColor: modelData.textColor
                    selected: dex.selectedAttributes.indexOf(modelData.key) >= 0
                    compact: true
                    onClicked: dex.toggleAttribute(modelData.key)
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 36
            radius: 7
            color: "#f1f5f8"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 4
                spacing: 4

                SegmentButton {
                    label: "任一属性"
                    checked: dex.attributeMode === "any"
                    onClicked: dex.attributeMode = "any"
                }

                SegmentButton {
                    label: "全部属性"
                    checked: dex.attributeMode === "all"
                    onClicked: dex.attributeMode = "all"
                }
            }
        }

        Separator {}

        SectionTitle { label: "阶段" }

        ComboBox {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            model: dex.stages
            currentIndex: Math.max(0, dex.stages.indexOf(dex.stage))
            onActivated: dex.stage = currentText
        }

        Switch {
            id: distributedSwitch
            Layout.fillWidth: true
            text: "只看有分布记录"
            checked: dex.onlyDistributed
            onToggled: dex.onlyDistributed = checked
        }

        Separator {}

        SectionTitle { label: "排序" }

        ComboBox {
            id: sortCombo
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            textRole: "label"
            valueRole: "key"
            model: root.sortModel
            currentIndex: root.sortIndex()
            onActivated: dex.sortKey = root.sortModel[currentIndex].key
        }

        Item {
            Layout.fillHeight: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 42
            radius: 7
            color: clearMouse.containsMouse ? "#fff7ed" : "#f7f9fb"
            border.color: "#d9e1ea"

            Text {
                anchors.centerIn: parent
                text: "清空筛选"
                color: "#9a3412"
                font.pixelSize: 13
                font.bold: true
            }

            MouseArea {
                id: clearMouse
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: dex.clearFilters()
            }
        }
    }

    component Separator: Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 1
        color: "#e4eaf1"
    }

    component SectionTitle: Text {
        property string label: ""
        text: label
        color: "#334155"
        font.pixelSize: 13
        font.bold: true
    }

    component SegmentButton: Rectangle {
        property string label: ""
        property bool checked: false
        signal clicked()

        Layout.fillWidth: true
        Layout.fillHeight: true
        radius: 5
        color: checked ? "#ffffff" : "transparent"
        border.color: checked ? "#d9e1ea" : "transparent"

        Text {
            anchors.centerIn: parent
            text: label
            color: parent.checked ? "#172033" : "#697386"
            font.pixelSize: 12
            font.bold: parent.checked
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: parent.clicked()
        }
    }
}
