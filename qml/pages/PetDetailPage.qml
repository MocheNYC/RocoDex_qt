import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

Rectangle {
    id: root
    property var pet: ({})
    readonly property bool hasPet: pet && pet.key

    radius: 8
    color: "#ffffff"
    border.color: "#cfd8e3"

    Flickable {
        id: flick
        anchors.fill: parent
        anchors.margins: 14
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        contentWidth: width
        contentHeight: detailColumn.implicitHeight

        ColumnLayout {
            id: detailColumn
            width: flick.width
            spacing: 14

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2

                    Text {
                        text: root.hasPet ? "精灵详情" : "请选择精灵"
                        color: "#111827"
                        font.pixelSize: 20
                        font.bold: true
                    }

                    Text {
                        visible: root.hasPet
                        text: root.hasPet ? "列表第 " + (pet.row + 1) + " 项" : ""
                        color: "#697386"
                        font.pixelSize: 12
                    }
                }

                ActionButton {
                    label: "上一只"
                    enabled: root.hasPet && pet.hasPrevious
                    onClicked: dex.selectPrevious()
                }

                ActionButton {
                    label: "下一只"
                    enabled: root.hasPet && pet.hasNext
                    onClicked: dex.selectNext()
                }

                ActionButton {
                    label: root.hasPet && pet.favorite ? "已收藏" : "收藏"
                    emphasized: root.hasPet && pet.favorite
                    enabled: root.hasPet
                    onClicked: dex.toggleFavorite()
                }
            }

            Rectangle {
                visible: root.hasPet
                Layout.fillWidth: true
                implicitHeight: heroLayout.implicitHeight + 28
                radius: 8
                color: "#f7f9fb"
                border.color: "#d9e1ea"

                RowLayout {
                    id: heroLayout
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 14
                    spacing: 18

                    Rectangle {
                        Layout.preferredWidth: 178
                        Layout.preferredHeight: 178
                        radius: 8
                        color: "#ffffff"
                        border.color: "#d9e1ea"

                        Image {
                            anchors.fill: parent
                            anchors.margins: 10
                            source: root.hasPet ? pet.image : ""
                            fillMode: Image.PreserveAspectFit
                            asynchronous: true
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 10

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Rectangle {
                                Layout.preferredHeight: 28
                                Layout.preferredWidth: idText.implicitWidth + 18
                                radius: 6
                                color: "#172033"

                                Text {
                                    id: idText
                                    anchors.centerIn: parent
                                    text: root.hasPet ? "NO." + pet.id : ""
                                    color: "#ffffff"
                                    font.pixelSize: 12
                                    font.bold: true
                                }
                            }

                            Rectangle {
                                Layout.preferredHeight: 28
                                Layout.preferredWidth: roleText.implicitWidth + 18
                                radius: 6
                                color: "#edf6fb"

                                Text {
                                    id: roleText
                                    anchors.centerIn: parent
                                    text: root.hasPet ? pet.roleLabel : ""
                                    color: "#2f6f8f"
                                    font.pixelSize: 12
                                    font.bold: true
                                }
                            }

                            Item { Layout.fillWidth: true }
                        }

                        Text {
                            Layout.fillWidth: true
                            text: root.hasPet ? pet.nameZh : ""
                            color: "#111827"
                            font.pixelSize: 34
                            font.bold: true
                            elide: Text.ElideRight
                            maximumLineCount: 1
                        }

                        Text {
                            Layout.fillWidth: true
                            text: root.hasPet && pet.nameEn ? pet.nameEn : "本地资料库"
                            color: "#697386"
                            font.pixelSize: 13
                            elide: Text.ElideRight
                            maximumLineCount: 1
                        }

                        Flow {
                            Layout.fillWidth: true
                            spacing: 7
                            Repeater {
                                model: root.hasPet ? pet.attributes : []
                                delegate: AttributeBadge {
                                    label: modelData.label
                                    badgeColor: modelData.color
                                    textColor: modelData.textColor
                                    clickable: false
                                }
                            }

                            PlainPill {
                                label: root.hasPet ? pet.stage : ""
                            }

                            PlainPill {
                                label: root.hasPet ? pet.petType : ""
                                dark: true
                            }
                        }

                        Text {
                            Layout.fillWidth: true
                            text: root.hasPet ? pet.introductionZh : ""
                            color: "#334155"
                            font.pixelSize: 13
                            wrapMode: Text.WordWrap
                            lineHeight: 1.18
                            maximumLineCount: 3
                            elide: Text.ElideRight
                        }
                    }
                }
            }

            GridLayout {
                id: detailGrid
                visible: root.hasPet
                Layout.fillWidth: true
                columns: root.width > 900 ? 2 : 1
                columnSpacing: 14
                rowSpacing: 14

                Section {
                    title: "资质"
                    subtitle: "与当前数据集最大值对比"
                    body: statsComponent
                }

                Section {
                    title: "属性克制"
                    subtitle: "防守弱点、抵抗与本属性进攻优势"
                    body: matchupComponent
                }

                Section {
                    title: "进化链"
                    subtitle: root.hasPet ? pet.evolutionRequirement : ""
                    body: evolutionComponent
                    wide: true
                }

                Section {
                    title: "特性"
                    subtitle: root.hasPet ? pet.traitName : ""
                    body: traitComponent
                }

                Section {
                    title: "体型"
                    subtitle: "身高、体重与类型"
                    body: bodyComponent
                }

                Section {
                    title: "分布"
                    subtitle: "公开地图记录"
                    body: distributionComponent
                }

                Section {
                    title: "图鉴介绍"
                    subtitle: "本地离线文本"
                    body: introComponent
                }

                Section {
                    title: "技能"
                    subtitle: root.hasPet ? pet.skills.length + " 条技能记录" : ""
                    body: skillsComponent
                    wide: true
                }

                Section {
                    title: "数据来源"
                    subtitle: root.hasPet ? pet.sourceKey : ""
                    body: sourceComponent
                    wide: true
                }
            }

            Rectangle {
                visible: !root.hasPet
                Layout.fillWidth: true
                Layout.preferredHeight: 260
                radius: 8
                color: "#f7f9fb"
                border.color: "#d9e1ea"

                Column {
                    anchors.centerIn: parent
                    width: Math.min(parent.width - 40, 340)
                    spacing: 10

                    Text {
                        width: parent.width
                        text: "当前没有可展示的条目"
                        color: "#111827"
                        font.pixelSize: 18
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                    }

                    Text {
                        width: parent.width
                        text: "请在左侧调整筛选条件，或清空筛选回到完整图鉴。"
                        color: "#697386"
                        font.pixelSize: 13
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }
    }

    component Section: Rectangle {
        property string title
        property string subtitle
        property Component body
        property bool wide: false

        Layout.fillWidth: true
        Layout.columnSpan: wide && detailGrid.columns > 1 ? 2 : 1
        implicitHeight: sectionColumn.implicitHeight + 24
        radius: 8
        color: "#ffffff"
        border.color: "#d9e1ea"

        ColumnLayout {
            id: sectionColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 12
            spacing: 10

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                Text {
                    Layout.fillWidth: true
                    text: title
                    color: "#111827"
                    font.pixelSize: 16
                    font.bold: true
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }

                Text {
                    visible: subtitle.length > 0
                    Layout.fillWidth: true
                    text: subtitle
                    color: "#697386"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                    maximumLineCount: 1
                }
            }

            Loader {
                Layout.fillWidth: true
                sourceComponent: body
            }
        }
    }

    component ActionButton: Rectangle {
        property string label: ""
        property bool emphasized: false
        signal clicked()

        Layout.preferredWidth: 78
        Layout.preferredHeight: 36
        radius: 7
        color: !enabled ? "#e6ebf1" : emphasized ? "#2f6f8f" : "#f7f9fb"
        border.color: emphasized ? "#2f6f8f" : "#d9e1ea"

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
            cursorShape: Qt.PointingHandCursor
            onClicked: parent.clicked()
        }
    }

    component PlainPill: Rectangle {
        property string label: ""
        property bool dark: false

        visible: label.length > 0
        width: pillText.implicitWidth + 18
        height: 30
        radius: 6
        color: dark ? "#172033" : "#ffffff"
        border.color: dark ? "#172033" : "#d9e1ea"

        Text {
            id: pillText
            anchors.centerIn: parent
            text: label
            color: dark ? "#ffffff" : "#334155"
            font.pixelSize: 13
            font.bold: true
        }
    }

    Component {
        id: statsComponent
        StatBars {
            stats: root.hasPet ? pet.stats : []
        }
    }

    Component {
        id: matchupComponent
        MatchupPanel {
            matchup: root.hasPet ? pet.matchup : ({})
        }
    }

    Component {
        id: evolutionComponent
        Flow {
            Layout.fillWidth: true
            spacing: 10
            Repeater {
                model: root.hasPet ? pet.evolutionChain : []
                delegate: Rectangle {
                    width: 170
                    height: 72
                    radius: 8
                    color: modelData.current ? "#edf6fb" : "#f7f9fb"
                    border.color: modelData.current ? "#2f6f8f" : "#d9e1ea"

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 8

                        Rectangle {
                            Layout.preferredWidth: 50
                            Layout.preferredHeight: 50
                            radius: 7
                            color: "#ffffff"
                            border.color: "#d9e1ea"

                            Image {
                                anchors.fill: parent
                                anchors.margins: 4
                                source: modelData.image
                                fillMode: Image.PreserveAspectFit
                                asynchronous: true
                            }
                        }

                        Text {
                            Layout.fillWidth: true
                            text: modelData.name
                            color: "#111827"
                            font.pixelSize: 13
                            font.bold: true
                            elide: Text.ElideRight
                            maximumLineCount: 2
                            wrapMode: Text.WordWrap
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: dex.selectPet(modelData.key)
                    }
                }
            }
        }
    }

    Component {
        id: traitComponent
        ColumnLayout {
            spacing: 8

            Text {
                Layout.fillWidth: true
                text: root.hasPet ? pet.traitDescription : ""
                color: "#334155"
                font.pixelSize: 13
                wrapMode: Text.WordWrap
                lineHeight: 1.18
            }
        }
    }

    Component {
        id: bodyComponent
        GridLayout {
            columns: 3
            columnSpacing: 10
            rowSpacing: 10

            Metric {
                label: "身高"
                value: root.hasPet ? pet.height : "-"
            }

            Metric {
                label: "体重"
                value: root.hasPet ? pet.weight : "-"
            }

            Metric {
                label: "类型"
                value: root.hasPet ? pet.petType : "-"
            }
        }
    }

    Component {
        id: distributionComponent
        Text {
            Layout.fillWidth: true
            text: root.hasPet ? pet.distributionZh : ""
            color: "#334155"
            font.pixelSize: 13
            wrapMode: Text.WordWrap
            lineHeight: 1.18
        }
    }

    Component {
        id: introComponent
        Text {
            Layout.fillWidth: true
            text: root.hasPet ? pet.introductionZh : ""
            color: "#334155"
            font.pixelSize: 13
            wrapMode: Text.WordWrap
            lineHeight: 1.18
        }
    }

    Component {
        id: skillsComponent
        ColumnLayout {
            spacing: 8
            Repeater {
                model: root.hasPet ? pet.skills : []
                delegate: SkillCard {
                    skill: modelData
                }
            }
        }
    }

    Component {
        id: sourceComponent
        RowLayout {
            spacing: 10

            Text {
                Layout.fillWidth: true
                text: root.hasPet ? pet.pageUrl : ""
                color: "#334155"
                font.pixelSize: 12
                elide: Text.ElideRight
                maximumLineCount: 1
            }

            ActionButton {
                label: "打开来源"
                enabled: root.hasPet && pet.pageUrl
                onClicked: Qt.openUrlExternally(pet.pageUrl)
            }
        }
    }

    component Metric: Rectangle {
        property string label
        property string value

        Layout.fillWidth: true
        Layout.preferredHeight: 62
        radius: 8
        color: "#f7f9fb"
        border.color: "#d9e1ea"

        Column {
            anchors.centerIn: parent
            width: parent.width - 12
            spacing: 3

            Text {
                width: parent.width
                text: value
                color: "#111827"
                font.pixelSize: 14
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
                maximumLineCount: 1
            }

            Text {
                width: parent.width
                text: label
                color: "#697386"
                font.pixelSize: 11
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
