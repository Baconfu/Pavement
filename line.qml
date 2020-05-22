import QtQuick 2.0
import Paint 1.0

Item {
    id: container;
    objectName: "container"
    x:10
    y:10
    width:100
    height:100
    z:5

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

        p0:Qt.point(0,0)
        p1:Qt.point(container.width,container.height)


        Rectangle{
            id:textBox
            objectName: "textBox"
            property int xModd;
            property int yModd;
            property int xModo;
            property int yModo;
            x: (xModd + xModo)/2 - textInput.contentWidth/2
            y: (yModd + yModo)/2 - textInput.contentHeight/2
            width: textInput.contentWidth;
            height: textInput.contentHeight;
            opacity: 0.8
            TextInput {
                x:0
                y:0
                id:textInput
                objectName: "textInput"
                onFocusChanged: {
                    if(focus)
                        forceActiveFocus()
                }

                text: ""
                font.pointSize: 9

            }
        }
    }
}
