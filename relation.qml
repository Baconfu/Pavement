import QtQuick 2.0
import Paint 1.0

Item {
    id: container;
    objectName: "container"
    x:10
    y:10
    width:100
    height:100
    z:-3

    PaintRelation {
        id: line
        objectName: "line"
        color:"grey"
        anchors.fill:parent

        p1:Qt.point(container.width,container.height)
        onP1Changed: {
            console.log(width,height,"width height");
        }
    }
}
