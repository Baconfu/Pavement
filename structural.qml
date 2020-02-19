import QtQuick 2.0
import Paint 1.0

Item {
    id: container
    x:0
    y:0
    width:100
    height:100
    z:-3


    PaintStructural{
        id: line
        objectName: "line"
        color: "grey"
        anchors.fill: parent
        p1:Qt.point(container.width,container.height)
    }
}
