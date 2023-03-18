#include "edge.h"
#include "scannode.h"
#include "graphwidget.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>

//! [0]
ScanNode::ScanNode(GraphWidget* graphWidget)
    : graph(graphWidget)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
    textItem = new QGraphicsTextItem("1", this);

}
//! [0]

//! [1]
void ScanNode::addEdge(Edge* edge)
{
    edgeList << edge;
    edge->adjust();
}

QList<Edge*> ScanNode::edges() const
{
    return edgeList;
}
//! [1]

//! [8]
QRectF ScanNode::boundingRect() const
{
    qreal adjust = 2;
    return QRectF(-10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
}
//! [8]

//! [9]
QPainterPath ScanNode::shape() const
{
    QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
    return path;
}
//! [9]

//! [10]
void ScanNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawEllipse(-7, -7, 20, 20);

    

    QRadialGradient gradient(-3, -3, 10);
    if (option->state & QStyle::State_Sunken) {
        gradient.setCenter(3, 3);
        gradient.setFocalPoint(3, 3);
        gradient.setColorAt(1, QColor(Qt::red).lighter(80));
        gradient.setColorAt(0, QColor(Qt::darkRed).lighter(120));
    }
    else {
        gradient.setColorAt(0, Qt::yellow);
        gradient.setColorAt(1, Qt::darkYellow);
    }
    painter->setBrush(gradient);

    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-10, -10, 20, 20);

    textItem->setPos(-9, -35);

}
void ScanNode::setIndex(int value)
{
    index = value;
    textItem->setPlainText(QString::number(index));
    QFont font("Segoe UI", 11);
    font.setBold(true);
    textItem->setFont(font);
}
//! [10]

//! [11]
QVariant ScanNode::itemChange(GraphicsItemChange change, const QVariant& value)
{
    switch (change) {
    case ItemPositionHasChanged:
        for (Edge* edge : std::as_const(edgeList))
            edge->adjust();
        graph->itemMoved();
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}
//! [11]

//! [12]
void ScanNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    update();
    graph->window()->setStatusTip("X pos: " + QString::number(this->scenePos().x()));

    QGraphicsItem::mousePressEvent(event);
    graph->setSelectedNode(this);
}

void ScanNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}