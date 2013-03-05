import QtQuick 1.0

Rectangle {
    property alias text: word.text
    property alias textColor: word.color
    property alias textVisible: word.visible
    property alias textVAlign: word.verticalAlignment
    property alias fontFamily: word.font.family

    color: "transparent"
    width: parent.width
    height: parent.height / 2

    Text {
        id: word
        width: parent.width
        height: parent.height
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: parent.height / 2.5
        scale: paintedWidth > parent.width ? (parent.width / paintedWidth) : 1
    }
}
