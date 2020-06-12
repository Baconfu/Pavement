import QtQuick 2.0
import QtQuick 2.12
import Paint 1.0
import QtQuick.Controls 2.0

Item {
    id: container;
    objectName: "container"
    property int absX
    property int absY





    z:0

    property bool expanded: false


    property bool highlighted: false
    property bool faded: false
    property bool ghost:false

    signal typeAccepted(string s)
    signal typePassivelyAccepted(string s)
    signal update()
    signal passivelyAccepted()


    onExpandedChanged: {
        textInput.updateInput();
        if(expanded){
            standard.visible = false
            expandedArea.visible = true
            typeNameContainer.y = expandedArea.y + expandedArea.height
            container.width = expandedArea.width
            container.height = nameContainer.height + expandedRectangle.height + typeNameContainer.height


        }
        else{
            standard.visible = true
            expandedArea.visible = false

            typeNameContainer.y = nameContainer.y + nameContainer.height
            textInput.updateInput()

            container.height = nameContainer.height + typeInput.height
            expandedRectangle.visible = false
            expandedTextBox.visible = false
        }
    }

    onHighlightedChanged: {
        if(highlighted){
            highlight.visible = true
        }
        else{
            highlight.visible = false
        }
    }
    onFadedChanged: {
        if(faded){
            opacity = 0.4
        }
        else{
            opacity = 1
        }
    }

    x:0
    y:0
    width:10
    height: nameContainer.height + typeInput.height




    onWidthChanged: {
        update()
        typeNameContainer.x = width/2 - typeInput.contentWidth/2
    }
    onHeightChanged: {
        update()
    }
    onXChanged: {
        update()
    }
    onYChanged:{
        update()
    }

    Rectangle{
        id: highlight
        visible: false
        anchors.fill: parent
        radius: 3
        color: "grey"
        z:-1
        opacity:0.2

    }

    Rectangle {
        id:nameContainer
        objectName: "nameContainer"
        color:"white"
        x:container.width/2 - width/2
        y:0
        width: 10
        z:-2

        height: textInput.contentHeight + 5


        TextInput {
            id: textInput
            x:5

            text: ""
            enabled: !ghost
            objectName: "textInput"
            activeFocusOnPress:false
            onContentWidthChanged: {
                updateInput()
            }

            Shortcut {
                sequence: "Ctrl+."

            }

            function updateInput(){
                if(contentWidth<5){
                    if(!expanded){
                        container.width = 10
                    }
                    nameContainer.width = 10
                }else{
                    if(!expanded){
                        container.width = contentWidth + 10
                    }
                    nameContainer.width = contentWidth + 10
                }
            }

            onFocusChanged: {
                if(!focus){
                    passivelyAccepted()
                }
            }

        }
        PaintNode {
            id: standard
            mode: "standard"
            anchors.fill: parent
            lineWidth: 2
            color: "grey"

        }

    }

    Rectangle{
        id: typeNameContainer
        objectName: "typeNameContainer"
        x:container.width / 2 - typeInput.contentWidth / 2
        y:nameContainer.height
        width:typeInput.contentWidth
        height:typeInput.contentHeight
        z:-3
        color: "white"
        TextInput {
            id: typeInput
            objectName: "typeName"
            activeFocusOnPress: false
            enabled: !ghost
            anchors.fill:parent
            font.pointSize: 9
            text: ""
            visible: false
            font.italic: false
            onContentWidthChanged: {
                typeNameContainer.x = container.width/2 - contentWidth/2
                if(contentWidth==0){
                    visible = false
                }else{
                    visible = true
                }
            }

            onFocusChanged: {
                if(focus == false){
                    typePassivelyAccepted(typeInput.text)
                }
            }
            onVisibleChanged: {
                console.log("what")
            }

            property bool italic: false
            onItalicChanged: {
                typeInput.font.italic = italic
            }

            onAccepted: {
                typeAccepted(typeInput.text)
            }
        }
    }
    ExpandIcon {
        visible:false
        x:container.width + 2
        y:4
        width:12
        height:12

    }
    Item {
        id: expandedArea
        objectName: "expandedArea"
        visible: false
        y:0
        x:0
        width: 100
        height:100
        onWidthChanged: {
            if(expanded){

                container.width = width
                typeNameContainer.x = width/2 - typeNameContainer.width/2
            }
        }
        onHeightChanged: {
            if(expanded){
                container.height = height + typeNameContainer.height
                typeNameContainer.y = height
            }
        }
        Rectangle {
            id: expandedRectangle
            objectName: "expandedRectangle"
            y:nameContainer.height
            x:0
            width:expandedArea.width
            height:expandedArea.height-y
            z:-1

            radius: 10
            border.color: "black"
            border.width: 1
        }
        Rectangle {
            visible: false

            onVisibleChanged: {
                expandedText.enabled = visible
                expandedRectangle.visible = !visible
            }

            id: expandedTextBox
            objectName: "expandedTextBox"
            y:nameContainer.height
            x:0
            width:expandedArea.width

            height:expandedArea.height-y
            z:-1
            opacity:0.8
            border.width:1
            border.color:"grey"
            TextArea {
                MouseArea{
                    anchors.fill:parent
                    onPressed: {
                        expandedText.cursorPosition = expandedText.positionAt(mouseX,mouseY)
                        mouse.accepted = true
                    }
                }

                selectByMouse: true
                onFocusChanged: {
                    if(focus){
                        cursorPosition = -1
                    }
                }

                tabStopDistance: 8
                enabled: parent.visible
                id: expandedText
                objectName: "expandedText"
                anchors.fill: parent
                font.pointSize: 9
                onContentWidthChanged: {
                    if(contentWidth > 80)
                        expandedArea.width = contentWidth + 20
                    else
                        expandedArea.width = 100
                }
                onContentHeightChanged: {
                    if(contentHeight > 60){
                        expandedArea.height = contentHeight + 40
                    }else{
                        expandedArea.height = 100
                    }
                }
            }
        }


    }


}
