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
        if(highlighted){
            console.log(10)
            line.lineWidth = 2
        }
        else{
            console.log(11)
            line.lineWidth = 1
        }
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
        Rectangle{
            id:textBox
            objectName: "textBox"
            property int xMod;
            property int yMod;
            x: xMod + (container.width-xMod)/2 - textInput.contentWidth/2
            y: yMod + (container.height-yMod)/2 - textInput.contentHeight/2
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
    ExpandIcon {
        visible: false
        x: container.width / 2 - 6
        y: container.height / 2 - 6
        width:12
        height:12



    }

}
