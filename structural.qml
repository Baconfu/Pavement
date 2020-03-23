import QtQuick 2.0
import Paint 1.0

Item {
    id: container
    x:0
    y:0
    width:100
    height:100
    z:-3

    property bool highlighted: false
    onHighlightedChanged: {
        if(highlighted)
            line.lineWidth = 2
        else
            line.lineWidth = 1
    }

    property int cutoffX
    property int cutoffY
    PaintStructural{

        id: line
        objectName: "line"
        color: "grey"
        anchors.fill: parent
        p1:Qt.point(container.width,container.height)



        lineWidth: 1

    }
    ExpandIcon {

        x: container.width / 2 - 6
        y: container.height / 2 - 6
        width:12
        height:12



    }

}
