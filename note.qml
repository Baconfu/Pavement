import QtQuick 2.0
import QtQuick.Controls 1.0

Rectangle {
    objectName: "container"
    id: container
    color:"white"
    width:100
    height:100
    TextArea {
        anchors.fill: parent
        id: textArea
        objectName: "textArea"

    }
}
