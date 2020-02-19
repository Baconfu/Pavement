#include "paintrelation.h"

PaintRelation::PaintRelation(QQuickItem * parent):
    QQuickItem (parent)
{
    setFlag(ItemHasContents,true);
}

PaintRelation::~PaintRelation()
{

}

QSGNode * PaintRelation::updatePaintNode(QSGNode * oldNode,UpdatePaintNodeData *)
{

    QSGGeometryNode *node = nullptr;
    QSGGeometry * geometry = nullptr;

    if(!oldNode){

        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(),6);
        geometry->setLineWidth(1);
        geometry->setDrawingMode(QSGGeometry::DrawLines);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(m_color);

        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    } else{
        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(m_color);

        node = static_cast<QSGGeometryNode *>(oldNode);
        geometry = node->geometry();
        geometry->setLineWidth(1);
        geometry->allocate(6);
        node->setMaterial(material);
    }

    const double pi = 3.1415;
    QSizeF s = size();

    double gradient = s.height() / s.width();
    double angle = atan(gradient);

    //quartiles
    bool top = false;
    bool right = false;

    if(s.height()>0){
        top = true;
    }
    if(s.width()>0){
        right = true;
    }
    if(!right){angle += pi;}
    if(right && !top){angle += 2*pi;}

    double arm = angle + (pi/6) ;
    double arm2 = angle - (pi/6);

    QPointF origin = QPointF(0,0);
    QPointF destination = p1();

    double armX = cos(arm) * 15;
    double armY = sin(arm) * 15;

    double arm2X = cos(arm2) * 15;
    double arm2Y = sin(arm2) * 15;

    QPointF armP = QPointF(p1().x() - armX,p1().y() - armY);
    QPointF arm2P = QPointF(p1().x() - arm2X,p1().y() - arm2Y);


    QSGGeometry::Point2D * vertices = geometry->vertexDataAsPoint2D();

    vertices[0].set(origin.x(),origin.y());
    vertices[1].set(destination.x(),destination.y());
    vertices[2].set(destination.x(),destination.y());
    vertices[3].set(armP.x(),armP.y());
    vertices[4].set(destination.x(),destination.y());
    vertices[5].set(arm2P.x(),arm2P.y());

    node->markDirty(QSGNode::DirtyGeometry);
    return node;
}
