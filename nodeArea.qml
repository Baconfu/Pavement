import QtQuick 2.0
import Paint 1.0

Item {
    id: container
    objectName: "container"

    x:0
    y:0
    z:1
    width:50
    height:50

    signal update();

    onXChanged: {
        update()
    }
    onYChanged: {
        update()
    }
    onWidthChanged: {
        update()
    }
    onHeightChanged: {
        update()
    }

    property bool highlighted: false
    onHighlightedChanged: {
        if(highlighted){
            highlight.visible = true
        }else{
            highlight.visible = false
        }
    }

    PaintNodeArea {
        id: paint
        anchors.fill: parent

    }
    Rectangle {
        id:highlight
        radius:3
        visible: false
        anchors.fill:parent
        opacity:0.2
        z:1
        color:"grey"
    }
}
