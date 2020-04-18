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




class Node;
class Relation;
class structural;
class GhostNode;

class Body: public QObject
{
    Q_OBJECT
    static Body * instance;

public:

    Body(QObject * parent = nullptr);
    static Body * getInstance();
    QQuickItem * getRoot();
    QQmlApplicationEngine * engine();
    QQuickWindow * window();
    void setFocusWindow();
    void frameView();



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
            return angle;
        }
    }coordinate;


    coordinate mousePosition(){return m_mousePosition;}
    coordinate tabPosition(){return m_tabPosition;}

    Node * getNodePointerByID(int id);
    Node * getNodePointerByID(int id,QVector<Node*> pool);
    Node * getNodeByName(QString name);
    QVector<Node*> getNodeByType(QString type);
    QVector<Node*> getNodeByType(Node * typeNode);

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
    void contextResolved(){m_context.pop_back();
                           //qDebug()<<m_context;
                          }
    void contextReset(){m_context.clear();}
    void setContext(int c){

        if(latestContext() != c){

            m_context.append(c);
            //qDebug()<<"setting context: "<<m_context;
        }

    }

    void abstract(QVector<Node*> nodes);

private:
    QString defaultPath = "/home/chuan/qt_projects/Pavement_1_1/saves";

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
    void pan(int x, int y);


    /*
     * idea: compound context system
     * context stored in linked list. mix and match of context numbers creates new emergent contexts.
     * pros: less typing, less clutter, potential for more specific contexts
     * cons: more complicated.
     */


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
        node_browsing = 12
    };

    QVector<int> m_context;

    Relation * m_hoveringRelation = nullptr;
    Relation * hoveringRelation(){return m_hoveringRelation;}
    void setHoveringRelation(Relation * r);

    coordinate m_mouseLocalPosition;
    coordinate m_mousePosition;
    coordinate m_tabPosition;

    QVector<Node*> nodeMap;
    QVector<Relation*> relationArchive;
    QVector<structural*> structuralMap;
    QVector<GhostNode*> ghostNodeMap;

    QVector<structural*> getAllStructurals();
    void updateStructuralMap();


    Node * m_selectedNode = nullptr;
    GhostNode * m_selectedGhost = nullptr;
    QVector<Node*> m_batchSelected;

    QVector<Node*> batchSelected(){
        return m_batchSelected;
    }
    void batchSelect(Node * n);
    void batchSelect(GhostNode * g);
    void batchSelect(QVector<Node*> n);
    void batchSelect(QVector<GhostNode*> g);
    void batchDeselect(Node * n);
    void batchDeselect(GhostNode * g);
    void batchDeselect(QVector<Node*> n){
        for(int i = 0; i<n.length(); i++){
            batchDeselect(n[i]);
        }
    }
    void batchDeselect(){
        QVector<Node*> temp;
        temp = m_batchSelected;
        for(int i=0; i<temp.length(); i++){
            batchDeselect(temp[i]);
        }

    }

    Relation * m_selectedRelation = nullptr;

    Node * m_highlightedNode = nullptr;
    GhostNode * m_highlightedGhost = nullptr;

    Node * selectedNode(){return m_selectedNode;}
    GhostNode * selectedGhost(){return m_selectedGhost;}
    Relation * selectedRelation(){return m_selectedRelation;}
    int selectedType(){
        qDebug()<<10;
        if(m_selectedNode){return 0;}
        if(m_selectedRelation){return 1;}
        if(m_selectedGhost){return 2;}
        return -1;
    }
    void setSelected(Node * n){
        m_selectedNode = n;
        m_selectedGhost = nullptr;
        m_selectedRelation = nullptr;
        if(n){
            setContext(Context::node_selected);
        }

    }
    void setSelected(GhostNode * g){
        m_selectedGhost = g;
        m_selectedNode = nullptr;
        m_selectedRelation = nullptr;
        if(g){
            setContext(Context::node_selected);
        }
    }
    void setSelected(Relation * r){
        m_selectedRelation = r;
        m_selectedNode = nullptr;
        m_selectedGhost = nullptr;
        if(r){
            setContext(Context::relation_selected);
        }
    }

    Node * highlightedNode(){return m_highlightedNode;}
    void setHighlightedNode(Node * n);
    void setHighlightedNode(GhostNode * n);
    void setHighlightedNode();
    QString highlightedNodeType(){if(m_highlightedNode){return "node";}if(m_highlightedGhost){return "ghost";}}


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
    void updateSearchBar(QStringList topMatches);
    void clearSearch();


    Node * newNode(int id, QString name,int x, int y,Node * parent, Node * typeNode);
    Node * newNode(int id, QString name, int x, int y);
    void newRelation(int id, Node * origin, Node * destination);
    void newRelation(int id, Node * origin, Relation * destination);
    void newRelation(int id, Relation * origin, Relation * destination);


    GhostNode * newGhostNode(Node * original,int x,int y);





    int allocateNewID(QString type);
signals:
    void mouseMoved();
public slots:
    void autoTab(int context);
    void tab();
    void enterPressed();
    void mouseTransform(int x,int y,int offsetX,int offsetY);
    int searching(QString input);
    int acceptedSelection(int n);
    void timeOut();

};

#endif // BODY_H
