import QtQuick 2.0
import Paint 1.0

Item {

    id: container

    signal close()

    width: tip.contentWidth + close.width + 15
    height: tip.contentHeight +10

    z:10

    Rectangle{
        id: bg
        objectName: "bg"

        anchors.fill:parent

        color: "grey"
        radius: 10

        z:5
    }

    Text {
        x:5
        y:5
        text: "This is a hot tip"
        id: tip
        objectName: "tip"

    }

    PaintCross{
        x: tip.contentWidth + 10
        y: container.height/2 - height/2
        width: 10
        height: 10
        lineWidth: 1

        MouseArea {
            anchors.fill: parent
            id: closeDetect
            objectName: "closeDetect"
            propagateComposedEvents: false
            onClicked: {
                close()
            }
        }
    }


}
