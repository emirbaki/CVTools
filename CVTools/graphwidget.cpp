#include "graphwidget.h"

#include "edge.h"
#include "scannode.h"

#include <math.h>

#include <QtWidgets>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <iostream>


//! [0]
GraphWidget::GraphWidget(QWidget* parent)
    : QGraphicsView(parent)
{
    _scene = new QGraphicsScene(this);
    _scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    _scene->setSceneRect(-200, -200, 400, 400);
  
    
    setGeometry(QRect(60, 40, 401, 451));
    setScene(_scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));

}
//! [1]

//! [2]
void GraphWidget::itemMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);
}
void GraphWidget::setSelectedNode(ScanNode* node)
{
    selectedNode = node;
}
void GraphWidget::setPicture(QImage& pixmap)
{
    pixmapItem = new QGraphicsPixmapItem;
    pixmapItem->setPixmap(QPixmap::fromImage(pixmap));
    auto rect = pixmapItem->boundingRect();
    pixmapItem->setZValue(-2);
    pixmapItem->setOffset(-(rect.width() / 2), -(rect.height() / 2));
    pixmapItem->setPos(0, 0);
    _scene->addItem(pixmapItem);
    _image = pixmap;

}
void GraphWidget::handleEdges()
{
    if (nodeList.count() < 2) return;
    
    for (int i = nodeList.count() - 1; i > -1; i--) {
        if (i == 0) {
            Edge* edge = new Edge(nodeList[i], nodeList.last());
            if (nodeList.last()->edges()[0]->destNode() == nodeList[i]) continue;
            _scene->addItem(edge);

        }
        else {
            Edge* edge = new Edge(nodeList[i], nodeList[i - 1]);
            _scene->addItem(edge);
        }
    }
}

void GraphWidget::warpPerspective(QLabel* item)
{
    std::vector<cv::Point2f> srcPoint, dstPoint;
    auto resp_w = pixmapItem->pixmap().width() / 2;
    auto resp_h = pixmapItem->pixmap().height() / 2;

    int w = 350, h = 500;

    dstPoint.push_back(cv::Point2f(0.0f, 0.0f));
    dstPoint.push_back(cv::Point2f(w, 0.0f));
    dstPoint.push_back(cv::Point2f(w, h));
    dstPoint.push_back(cv::Point2f(0.0f, h));

    for (int i = 0; i < nodeList.count(); i++) {
        QPointF scenePoz = nodeList[i]->scenePos();
        cv::Point2f pos(scenePoz.x() + resp_w, scenePoz.y() + resp_h);
        std::cout << pos.x + pos.y;
        srcPoint.push_back(pos);
    }
    cv::Mat transform = cv::getPerspectiveTransform(srcPoint, dstPoint);

    // Warp the perspective using the transformation matrix and display the result in the QGraphicsView
    auto a = _image.width();

    cv::Mat image = CVToolsUtility::qimage_to_mat(_image);
    cv::Mat warpedImage;
    cv::warpPerspective(image, warpedImage, transform, image.size() / 2);

    QImage qWarpedImage(warpedImage.data, warpedImage.cols, warpedImage.rows, QImage::Format_RGBA8888);
    QPixmap pixmap = QPixmap::fromImage(qWarpedImage);
    item->setPixmap(pixmap);
}
//! [2]

//! [3]
void GraphWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        selectedNode->moveBy(0, -20);
        break;
    case Qt::Key_Down:
        selectedNode->moveBy(0, 20);
        break;
    case Qt::Key_Left:
        selectedNode->moveBy(-20, 0);
        break;
    case Qt::Key_Right:
        selectedNode->moveBy(20, 0);
        break;
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Space:
    default:
        QGraphicsView::keyPressEvent(event);
    }
}
//! [3]

//! [4]
void GraphWidget::timerEvent(QTimerEvent* event)
{
    Q_UNUSED(event);

    QList<ScanNode*> nodes;
    const QList<QGraphicsItem*> items = _scene->items();
    for (QGraphicsItem* item : items) {
        if (ScanNode* node = qgraphicsitem_cast<ScanNode*>(item))
            nodes << node;
    }

    //for (ScanNode* node : std::as_const(nodes))
        //node->calculateForces();

    /*bool itemsMoved = false;
    for (ScanNode* node : std::as_const(nodes)) {
        if (node->advancePosition())
            itemsMoved = true;
    }*/

    /*if (!itemsMoved) {
        killTimer(timerId);
        timerId = 0;
    }*/
}
//! [4]

#if QT_CONFIG(wheelevent)
//! [5]
void GraphWidget::wheelEvent(QWheelEvent* event)
{
    scaleView(pow(2., -event->angleDelta().y() / 240.0));
}
//! [5]
#endif

//! [6]
void GraphWidget::drawBackground(QPainter* painter, const QRectF& rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
        painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
        painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

    

    // Text
    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
        sceneRect.width() - 4, sceneRect.height() - 4);
    QString message(tr("Click and drag the nodes around, and zoom with the mouse "
        "wheel or the '+' and '-' keys"));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
    painter->drawText(textRect.translated(2, 2), message);
    painter->setPen(Qt::black);
    painter->drawText(textRect, message);
}
//! [6]

//! [7]
void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}
//! [7]

void GraphWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void GraphWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}