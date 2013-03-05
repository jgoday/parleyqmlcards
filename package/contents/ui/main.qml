import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    property string source
    property string lang1
    property string lang2
    property int intervalValue

    Connections {
        target: plasmoid

        onConfigUpdated: {
            configChanged()
        }
    }

    Component.onCompleted: {
        configChanged()
    }

    function configChanged() {
        original.fontFamily = plasmoid.font
        translated.fontFamily = plasmoid.font
        intervalValue = plasmoid.updateInterval
        source = plasmoid.fileName
        lang1 = plasmoid.lang1
        lang2 = plasmoid.lang2
        original.textVisible = plasmoid.alwaysRevealSolution
        dataSource.connectSource(source)
    }

    PlasmaCore.DataSource {
        id: dataSource
        engine: "parley"
        interval: intervalValue

        function refresh() {
            var src = source
            disconnectSource(src)
            interval = 100
            connectSource(src)
        }

        onNewData: {
            translated.text = data[lang1]
            original.text = data[lang2]
            if (interval != intervalValue) {
                interval = intervalValue
            }
        }
    }

    Image {
        // source: "plasmapackage:/images/card.svg"
        source: "../images/card.svg"
        sourceSize.width: parent.width
        sourceSize.height: parent.height - footer.height
    }

    Rectangle {
        height: parent.height
        width: parent.width - 20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
        color: "transparent"

        WordRow {
            id: translated
        }

        WordRow {
            id: original
            y: parent.height / 2
            textColor: "gray"
            textVisible: plasmoid.alwaysRevealSolution
            textVAlign: Text.AlignTop
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            anchors.margins: -10
            hoverEnabled: true
            // onClicked: original.textVisible = true
            onEntered: original.textVisible = true
            onExited: original.textVisible = plasmoid.alwaysRevealSolution
            onPressed: {
                mouse.accepted = true
            }
        }
    }

    Rectangle {
        id: footer
        property int spacing: 10
        width: parent.width
        height: nextButton.height + spacing
        y: original.y + original.height - (height)
        anchors.horizontalCenter: parent.horizontalCenter
        color: Qt.rgba(0, 0, 0, 0.3)

        PlasmaComponents.Button {
            id: nextButton
            property int spacing: 5

            text: i18n("Next")
            x: original.x + original.width - width
            anchors.verticalCenter: parent.verticalCenter

            onClicked: {
                dataSource.refresh()
            }
        }
    }
}
