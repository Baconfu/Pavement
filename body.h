#ifndef BODY_H
#define BODY_H

#include <QColor>
#include <QObject>
#include <QQuickItem>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTimer>
#include <QDebug>
#include <QQmlComponent>
#include <QStringList>
#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <dirent.h>
#include <QElapsedTimer>
#include <utility.h>
#include <QDir>
#include <QSysInfo>



class tip;
class BaseNode;
class Node;
class NodeArea;
class Relation;
class GhostNode;
class Note;

class Body: public QObject
{
    Q_OBJECT
    static Body * instance;

public:


    Body(QObject * parent = nullptr);

    QString os(){return QSysInfo::productType();}

    static Body * getInstance();
    QQuickItem * getRoot();
    QQmlApplicationEngine * engine();
    QQuickWindow * window();
    void fullscreen(bool b);
    void setFocusWindow();
    void frameView();


    void zoom(double deltaFactor);

    bool switchvar = false;

    void initialize();
    void saveFile(QString path);
    void openFile(QString path);

    QStringList getSaves(QString path);


    enum cellStyle{
        minimal = 0,
        closed = 1,
        noBorder = 2
    };
    enum relationType{
        outgoing = 1,
        incoming = 0,
        toRelation = 2,
        toNode = 3
    };

    typedef struct style{
        QColor textColor;
        QColor fillColor;
        QColor borderColor;

        int fontSize;
        int cellStyle;

    }style;

    style defaultStyle(){
        style s;
        s.textColor = QColor::fromRgb(0,0,0);
        s.fillColor = QColor::fromRgb(255,255,255,0);
        s.borderColor = QColor::fromRgb(0,0,0);
        s.fontSize = 12;
        s.cellStyle = cellStyle::minimal;
        return s;
    }

    enum responseAction {
        do_nothing = 0,
        hover_select = 1,
        hover_deselect = 2,
        select_expandedText = 3

    };

    typedef struct response{
        bool isInside = false;
        int action = do_nothing;
        BaseNode * node = nullptr;
    }response;


    typedef struct coordinate{
        int x;
        int y;
        coordinate subtract(coordinate c){
            coordinate r;
            r.x = x - c.x;
            r.y = y - c.y;
            return r;
        }
        double angle;
        double getAngle(){
            angle = atan(double(y)/double(x));
            if(x<0){
                angle += 3.1415;
            }
            if(x>0 && y<0){
                angle += 3.1415 * 2;
            }
            if(x==0){
                angle = 3.1415 / 2;
                if(y < 0){
                    angle = 3 * 3.1415 / 2;
                }
            }
            return angle;
        }
        coordinate add(coordinate c){
            coordinate r;
            r.x = x + c.x;
            r.y = y + c.y;
            return r;
        }
        coordinate addX(int a){
            coordinate r;
            r.x = x + a;
            r.y = y;
            return r;
        }
        coordinate addY(int a){
            coordinate r;
            r.x = x;
            r.y = y + a;
            return r;
        }
        coordinate invert(){
            coordinate r;
            r.x = x * -1;
            r.y = y * -1;
            return r;
        }
        operator QPoint() const {return QPoint(x,y);}
    }coordinate;


    coordinate averagePosition(QVector<BaseNode*> nodes);
    coordinate medianPosition(QVector<BaseNode*> nodes);
    QVector<coordinate> boundaries(QVector<BaseNode*> nodes);

    coordinate mousePosition(){return m_mousePosition;}
    coordinate oldMousePosition(){return m_oldMousePosition;}
    coordinate mouseVector(){
        return m_mouseVector;
    }
    bool mouseInWindow(){return m_mouseInWindow;}
    coordinate tabPosition(){return m_tabPosition;}

    Node * newNode(int id, QString name,int x, int y,Node * parent, Node * typeNode);
    Node * newNode(int id, QString name, int x, int y);
    Relation * newRelation(int id, BaseNode * origin, BaseNode * destination);
    Relation * newRelation(int id, BaseNode * origin, Relation * destination);
    Relation * newRelation(int id, Relation * origin, Relation * destination);
    Relation * newLine(int id, BaseNode * origin, BaseNode * destination);
    Relation * newTriangle(int id, BaseNode * origin, BaseNode * destination);

    GhostNode * newGhostNode(Node * original,int x,int y);

    NodeArea * newNodeArea(QVector<BaseNode*> nodes);
    NodeArea * newNodeArea(BaseNode * n);

    Note * newNote(int id,int x,int y);



    BaseNode * getNodePointerByID(int id);
    BaseNode * getNodePointerByID(int id,QVector<BaseNode*> pool);
    Node * getNodeByName(QString name);
    Node * getNodeByInfo(QString name,QString type);
    QVector<Node*> getNodeByType(QString type);
    QVector<Node*> getNodeByType(Node * typeNode);

    bool nameAlreadyExists(QString name,BaseNode * asker);

    void registerGhost(GhostNode * g);

    void removeNode(Node * n);
    void removeGhost(GhostNode * g);
    void removeRelation(Relation * r);
    void removeNode(BaseNode * b);

    void showTip(QString s);
    void removeTip(tip * t);
    void autoRemoveTip(QString s);

    BaseNode * getCommonAbstraction(QVector<BaseNode*> nodes);


    Relation * getRelationPointerByID(int id);
    Relation * getRelationPointerByID(int id,QVector<Relation*> pool);

    coordinate getDisplayDimensions();


    QVector<int> context(){return m_context;}
    int latestContext(){
        if(m_context.length()==0){
            return -1;
        }
        return m_context[m_context.length()-1];
    }
    void contextResolved(){
        if(true){
            //qDebug()<<"context resolved:"<<m_context;
        }
        m_context.pop_back();
    }
    void contextReset(){

        while(m_context.length()){
            contextResolved();
        }
    }
    void setContext(int c){
        if(c == tab_searching){
            if(latestContext() != c){

                m_context.append(c);

            }

        }else{
            if(!context().contains(c)){
                m_context.append(c);

            }
        }

    }

    Node * getNodeFromBase(BaseNode * b);
    GhostNode * getGhostFromBase(BaseNode * b);

    void abstract(QVector<BaseNode*> nodes);
    enum Context{
        neutral = 0,
        node_selected = 1,
        relation_selected = 2,
        nothing_selected = 3,
        creating_relation = 4,
        parenting = 5,
        tab_searching = 6,
        opening_file = 7,
        saving_file = 8,
        including = 9,
        batch_selected = 10,
        batch_selecting = 11,
        node_browsing = 12,
        moving_node = 13,
        structural_selected = 14,
        mouse_held = 15,
        dragging_camera =16,
        getting_node = 17,
        connection_mode = 18,
        copying_submap = 19
    };

    int allocateNewID(QString type);
private:
    QVector<bool> freshBoot;
    enum freshBootParameter{
        first_tab=0,
        first_node=1,
        first_relation=2,
        first_batch=3,
        first_abstraction=4,
        first_note=5
    };

    int sessionLength = 0;

    QString currentFile = "";
    QString defaultPath = QDir::currentPath() + "/saves";

    QTimer autosaveTimer;
    void startAutosaveTimer();
    void stopAutosaveTimer();



    QTimer timer;
    void startTimer();
    void stopTimer();

    int tally = 0;
    int debugTally = 1;

    Body::coordinate m_velocity;
    double velocityMagnitude(int x,int y);
    coordinate padding(int screenWidth, int screenHeight);
    coordinate padding(coordinate c);
    bool inBounds(int x,int y);
    bool draggingCamera(){return m_draggingCamera;}
    void dragCamera(bool b){m_draggingCamera = b;}
    void pan(int x, int y);


    /*
     * idea: compound context system
     * context stored in linked list. mix and match of context numbers creates new emergent contexts.
     * pros: less typing, less clutter, potential for more specific contexts
     * cons: more complicated.
     */




    QVector<int> m_context;

    Relation * m_hoveringRelation = nullptr;
    Relation * hoveringRelation(){return m_hoveringRelation;}
    void setHoveringRelation(Relation * r);




    double m_zoomFactor = 1;
    double m_zoomVelocity = 0;
    coordinate m_mouseScreenSpacePosition;
    coordinate m_oldMouseScreenSpaceScaledPosition;
    coordinate m_mousePosition;
    coordinate m_oldMousePosition;
    coordinate m_mouseVector;
    coordinate m_tabPosition;
    bool m_mouseHeld = false;
    bool m_scrolling = false;
    bool m_scaling = false;
    bool m_mouseInWindow = true;
    bool m_draggingCamera = false;


    QVector<BaseNode*> nodeMap;
    QVector<Relation*> relationArchive;


    QVector<tip*> allTips;



    void removeGhosts(QVector<GhostNode*> ghosts);
    void removeNodes(QVector<BaseNode*> nodes);


    BaseNode * target_node = nullptr;
    BaseNode * m_currently_hovering = nullptr;
    BaseNode * m_selectedNode = nullptr;
    QVector<BaseNode*> m_batchSelected;
    QVector<BaseNode*> batchSelected(){
        return m_batchSelected;
    }
    void batchSelect(BaseNode * n);
    void batchSelect(QVector<BaseNode*> n);
    void batchDeselect(BaseNode * n);
    void batchDeselect(QVector<BaseNode*> n){
        for(int i = 0; i<n.length(); i++){
            batchDeselect(n[i]);
        }
    }
    void batchDeselect(){
        QVector<BaseNode*> temp;
        temp = m_batchSelected;
        for(int i=0; i<temp.length(); i++){
            batchDeselect(temp[i]);
        }

    }

    Relation * m_selectedRelation = nullptr;

    BaseNode * m_highlightedNode = nullptr;


    BaseNode * selectedNode(){return m_selectedNode;}
    Relation * selectedRelation(){return m_selectedRelation;}
    void setSelected(BaseNode * n);
    void setSelected(Relation * r){
        m_selectedRelation = r;


        if(r){

            m_selectedNode = nullptr;
            setContext(Context::relation_selected);
        }
    }

    BaseNode * highlightedNode(){return m_highlightedNode;}
    void setHighlightedNode(BaseNode * n);
    void setHighlightedNode();




    struct function {
        QString name;
        QStringList alias;
        QString commonShorthand;
        QVector<int> contexts;
        int match;
        int frequency;
    };

    QVector<function> functions;
    QVector<function> displayFunctions;

    QVector<function> functionFromList(QStringList l);

    int match(function * f, QString input);
    int matchString(QString s, QString input);

    QQuickItem * m_searchBar = nullptr;
    int m_searchBar_defaultWidth = 186;
    void updateSearchBar(QStringList topMatches);
    void clearSearch();
    void cycleSearchBarSelection();

    bool tabAccepted = false; //mutual exclusion: tabaccepted and enterPressed






signals:
    void mouseMoved();
public slots:
    void autosave();
    void autoTab(int context);
    void tab();
    void enterPressed();
    void escapePressed();
    void scroll(int x,int y,bool ctrl);

    void windowChanged();

    void mouseTransform(int x,int y,int offsetX,int offsetY);
    int searching(QString input);
    int acceptedSelection(int n);
    void timeOut();

    void mouseClicked(int x,int y);
    void mouseDoubleClicked(int x,int y);
    void mousePressed(int x,int y);
    void mouseReleased();
    void mouseHeld();
    void mouseInWindowChanged(bool b);

    void closeWindow();

};

#endif // BODY_H
