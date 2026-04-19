#include "visualnode.h"
#include "arrow.h"
#include "mainwindow.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QHBoxLayout>

// конструктор
VisualNode::VisualNode(int id, QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent),
    nodeID(id),
    calculated(false),
    value(0)
{
    setRect(-35, -35, 70, 70);


    setZValue(0);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    QColor color;
    color = QColor(255, 230, 255);
    setBrush(color);
    setPen(QPen(Qt::black, 2));
}