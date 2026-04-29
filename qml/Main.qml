import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    width: 1440
    height: 900
    minimumWidth: 1120
    minimumHeight: 720
    visible: true
    title: "RocoDex Qt"
    color: "#edf1f5"

    font.family: "Microsoft YaHei UI"
    font.pixelSize: 13

    DexPage {
        anchors.fill: parent
    }
}
