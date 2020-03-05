import QtQuick 2.0

Item {
    id: item
    x:0
    y:0
    z:1
    property bool selectFirst: true
    property int optionCount: 0
    onOptionCountChanged: rectangle.displayCount = optionCount
    width: rectangle.width
    height: rectangle.height
    property int optionHighlighted:-1
    onOptionHighlightedChanged: {
        if(optionHighlighted!=-1)
            highlight.visible = true
        else
            highlight.visible = false
        highlight.y = 33 + optionHighlighted * 30
        if(optionHighlighted>4){
            optionHighlighted=0
        }
    }

    signal optionSelected(int selection)
    signal searching(string input)

    onVisibleChanged: {
        if(visible == false){
            textInput.text = ""
            searching("")
        }
    }

    Rectangle {
        id: rectangle
        x: 0
        y: 0
        width: 186
        height: 33 + displayCount * 30
        color: "lightGrey"
        property int displayCount:0;
        onDisplayCountChanged: {
            if(displayCount > 0){element.visible = true}
            else{element.visible = false}
            if(displayCount > 1){element1.visible = true}
            else{element1.visible = false}
            if(displayCount > 2){element2.visible = true}
            else{element2.visible = false}
            if(displayCount > 3){element3.visible = true}
            else{element3.visible = false}
            if(displayCount > 4){element4.visible = true}
            else{element4.visible = false}
        }

        TextInput {
            id: textInput
            objectName:"textInput"
            x: 13
            y: 7
            width: 80
            height: 20





            font.pixelSize: 18

            onTextChanged: {
                searching(text)
            }
            onAccepted: {
                if(text.length == 0){
                    optionSelected(-1)
                }
                if(item.optionHighlighted>-1){
                    optionSelected(item.optionHighlighted)
                }
                if(item.optionHighlighted==-1 && text.length>0){
                    if(selectFirst){
                        optionSelected(0)
                    }else{
                        optionSelected(-1)
                    }


                }


            }
        }

        Rectangle {
            id:highlight
            visible: false
            x:0
            y:0
            width:rectangle.width
            height:30
            color:"grey"
            opacity: 0.2
        }

        Text {
            id: element
            objectName: "option0"
            visible: false
            x: 13
            y: 37
            text: qsTr("Text")
            fontSizeMode: Text.FixedSize
            font.pixelSize: 16
        }

        Text {
            id: element1
            objectName: "option1"
            visible: false
            x: 13
            y: 67
            text: qsTr("Text")
            font.pixelSize: 16
            fontSizeMode: Text.FixedSize
        }

        Text {
            id: element2
            objectName: "option2"
            visible: false
            x: 13
            y: 97
            text: qsTr("Text")
            font.pixelSize: 16
            fontSizeMode: Text.FixedSize
        }

        Text {
            id: element3
            objectName: "option3"
            visible: false
            x: 13
            y: 127
            text: qsTr("Text")
            font.pixelSize: 16
            fontSizeMode: Text.FixedSize
        }

        Text {
            id: element4
            objectName: "option4"
            visible: false
            x: 13
            y: 157
            text: qsTr("Text")
            font.pixelSize: 16
            fontSizeMode: Text.FixedSize
        }
    }

}
