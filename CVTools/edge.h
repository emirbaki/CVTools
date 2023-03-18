#pragma once
#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsItem>

class ScanNode;

//! [0]
class Edge : public QGraphicsItem
{
public:
    Edge(ScanNode* sourceNode, ScanNode* destNode);

    ScanNode* sourceNode() const;
    ScanNode* destNode() const;

    void adjust();

    enum { Type = UserType + 2 };
    int type() const override { return Type; }

protected:
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    ScanNode* source, * dest;

    QPointF sourcePoint;
    QPointF destPoint;
    qreal arrowSize = 10;
};
//! [0]

#endif // EDGE_H
