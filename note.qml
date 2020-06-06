import QtQuick 2.0
import QtQuick.Controls 1.0

Item{
    objectName: "container"
    id: container
    width:100
    height:100
    Rectangle {
        objectName: "bg"
        id: bg
        color:"white"
        anchors.fill: parent
        border.width: 1
        border.color: "grey"

    }
    TextArea {
        anchors.fill: parent
        id: textArea
        objectName: "textArea"

    }
}


