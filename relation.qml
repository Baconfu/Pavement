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

    property bool highlighted: false
    onHighlightedChanged: {
        if(highlighted)
            line.lineWidth = 2
        else
            line.lineWidth = 1
    }

    PaintRelation {
        id: line
        objectName: "line"
        color:"grey"
        anchors.fill:parent
        lineWidth: 1

        p1:Qt.point(container.width,container.height)


        Rectangle{
            id:textBox
            objectName: "textBox"
            property int xMod;
            property int yMod;
            x: xMod/2 - textInput.contentWidth/2
            y: yMod/2 - textInput.contentHeight/2
            width: textInput.contentWidth;
            height: textInput.contentHeight;
            opacity: 0.8
            TextInput {
                x:0
                y:0
                id:textInput
                objectName: "textInput"
                text: ""
                font.pointSize: 9

            }
        }
    }
}
