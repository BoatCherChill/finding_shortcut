#include "diagramscene.h"
#include "arrow.h"
#include "graph.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QString>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>
#include <QCursor>
#include <QInputDialog>
#include <QMessageBox>
#include <cmath>
#include <map>

DiagramScene::DiagramScene(QMenu* itemMenu, QObject* parent)
    : QGraphicsScene(parent), my_menu(itemMenu), tempLine(nullptr), tempStartNode(nullptr), nextNodeId(1)
{
    setSceneRect(-5000, -5000, 10000, 10000);
    setBackgroundBrush(Qt::white);
}

void DiagramScene::setMode(Mode mode) {
    my_mode = mode;

    if (mode == InsertNode) {
        for (QGraphicsItem* item : items()) {
            if (QGraphicsEllipseItem* node = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {
                node->setFlag(QGraphicsItem::ItemIsMovable, true);
            }
        }
    }
    else if (mode == InsertArrow) {
        for (QGraphicsItem* item : items()) {
            if (QGraphicsEllipseItem* node = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {
                node->setFlag(QGraphicsItem::ItemIsMovable, false);
            }
        }
    }
    else if (mode == EditItems) {
        for (QGraphicsItem* item : items()) {
            if (QGraphicsEllipseItem* node = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {
                node->setFlag(QGraphicsItem::ItemIsMovable, true);
            }
        }
    }
}

void DiagramScene::createNode(const QPointF& pos) {
    QGraphicsEllipseItem* node = new QGraphicsEllipseItem(-30, -30, 60, 60);
    node->setPos(pos);
    node->setBrush(QBrush(QColor(200, 220, 255)));
    node->setPen(QPen(Qt::black, 2));
    node->setFlag(QGraphicsItem::ItemIsMovable, true);
    node->setFlag(QGraphicsItem::ItemIsSelectable, true);
    node->setData(0, nextNodeId - 1);

    QGraphicsTextItem* text = new QGraphicsTextItem(QString::number(nextNodeId), node);
    text->setDefaultTextColor(Qt::black);
    text->setFont(QFont("Arial", 12, QFont::Bold));
    text->setPos(-10, -10);

    addItem(node);
    nextNodeId++;
}

void DiagramScene::createArrow(QGraphicsEllipseItem* startNode, QGraphicsEllipseItem* endNode) {
    if (!startNode || !endNode || startNode == endNode) return;

    for (Arrow* a : arrows) {
        if (a->startItem() == startNode && a->endItem() == endNode) {
            return;
        }
    }

    Arrow* existingReverseArrow = nullptr;
    for (Arrow* a : arrows) {
        if (a->startItem() == endNode && a->endItem() == startNode) {
            existingReverseArrow = a;
            break;
        }
    }

    Arrow* arrow = new Arrow(startNode, endNode);

    bool ok;
    int weight = QInputDialog::getInt(nullptr, "Вес связи", "Введите вес (положительное число):", 1, 1, 1000, 1, &ok);
    if (!ok) {
        delete arrow;
        return;
    }

    arrow->setWeight(weight);
    arrow->setFullWeight(QString::number(weight));

    // Если есть обратная стрелка - делаем обе двойными
    if (existingReverseArrow) {
        existingReverseArrow->setDouble(true);

        int fromId = startNode->data(0).toInt();
        int toId = endNode->data(0).toInt();
        emit checkCycle(fromId, toId, nullptr);
        emit graphChanged();
    }
    else {
        arrows.append(arrow);
        addItem(arrow);

        int fromId = startNode->data(0).toInt();
        int toId = endNode->data(0).toInt();
        emit checkCycle(fromId, toId, arrow);
    }

    emit graphChanged();
}

void DiagramScene::editArrowWeight(Arrow* arrow) {
    if (!arrow) return;

    bool ok;
    int newWeight = QInputDialog::getInt(nullptr, "Редактирование веса",
        "Введите новый вес:", arrow->getWeight(), 1, 1000, 1, &ok);
    if (ok) {
        arrow->setWeight(newWeight);
        arrow->setFullWeight(QString::number(newWeight));  
        emit graphChanged();  
        update();
    }
}

void DiagramScene::updateAllArrows() {
    for (Arrow* arrow : arrows) {
        arrow->updatePosition();
    }
    update();
}

void DiagramScene::deleteSelectedItem() {
    QList<QGraphicsItem*> selected = selectedItems();
    if (selected.isEmpty()) return;

    for (QGraphicsItem* item : selected) {
        if (QGraphicsEllipseItem* node = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {

            QVector<Arrow*> arrowsToDelete;
            for (Arrow* arrow : arrows) {
                if (arrow->startItem() == node || arrow->endItem() == node) {
                    arrowsToDelete.append(arrow);
                }
            }

            for (Arrow* arrow : arrowsToDelete) {
                arrows.removeOne(arrow);
                removeItem(arrow);
                delete arrow;
            }

            removeItem(node);
            delete node;
        }

        else if (Arrow* arrow = qgraphicsitem_cast<Arrow*>(item)) {
            arrows.removeOne(arrow);
            removeItem(arrow);
            delete arrow;
        }
    }

    emit checkCycle(0, 0, nullptr);
    emit graphChanged();
    update();
}

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    if (my_mode == InsertNode) {
        createNode(event->scenePos());
    }
    else if (my_mode == InsertArrow) {
        QList<QGraphicsItem*> itemsAtPos = items(event->scenePos());
        for (QGraphicsItem* item : itemsAtPos) {
            if (QGraphicsEllipseItem* node = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {
                tempStartNode = node;
                tempLine = new QGraphicsLineItem(QLineF(event->scenePos(), event->scenePos()));
                tempLine->setPen(QPen(Qt::red, 2, Qt::DashLine));
                tempLine->setZValue(1000);
                addItem(tempLine);
                break;
            }
        }
    }
    else if (my_mode == EditItems) {

        QGraphicsScene::mousePressEvent(event);

        QList<QGraphicsItem*> itemsAtPos = items(event->scenePos());
        for (QGraphicsItem* item : itemsAtPos) {
            if (Arrow* arrow = qgraphicsitem_cast<Arrow*>(item)) {
                emit itemSelected(arrow);
                break;
            }
        }
    }
}

void DiagramScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    if (my_mode == EditItems) {

        QList<QGraphicsItem*> itemsAtPos = items(event->scenePos());
        for (QGraphicsItem* item : itemsAtPos) {
            if (Arrow* arrow = qgraphicsitem_cast<Arrow*>(item)) {
                editArrowWeight(arrow);
                break;
            }
        }
    }

    QGraphicsScene::mouseDoubleClickEvent(event);
}

void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (tempLine && tempStartNode) {
        tempLine->setLine(QLineF(tempStartNode->pos(), event->scenePos()));
    }

    updateAllArrows();

    QGraphicsScene::mouseMoveEvent(event);
}

void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (tempLine && tempStartNode) {
        QList<QGraphicsItem*> itemsAtPos = items(event->scenePos());
        QGraphicsEllipseItem* endNode = nullptr;

        for (QGraphicsItem* item : itemsAtPos) {
            if (QGraphicsEllipseItem* node = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {
                if (node != tempStartNode) {
                    endNode = node;
                    break;
                }
            }
        }

        removeItem(tempLine);
        delete tempLine;
        tempLine = nullptr;

        if (endNode) {
            createArrow(tempStartNode, endNode);
        }

        tempStartNode = nullptr;
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void DiagramScene::clearAll() {
    for (Arrow* arrow : arrows) {
        removeItem(arrow);
        delete arrow;
    }
    arrows.clear();

    QList<QGraphicsItem*> itemsToDelete;
    for (QGraphicsItem* item : items()) {
        if (QGraphicsEllipseItem* node = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {
            itemsToDelete.append(item);
        }
    }

    for (QGraphicsItem* item : itemsToDelete) {
        removeItem(item);
        delete item;
    }

    nextNodeId = 1;
}

void DiagramScene::loadGraph(const vector<pair<int, pair<int, int>>>& nodesData, const vector<GraphArrow>& arrowsData) {
    
    clearAll();

    map<int, QGraphicsEllipseItem*> nodeItems;

    for (const auto& nodeData : nodesData) {
        int nodeId = nodeData.first;
        int x = nodeData.second.first;
        int y = nodeData.second.second;

        QGraphicsEllipseItem* node = new QGraphicsEllipseItem(-30, -30, 60, 60);
        node->setPos(x, y);
        node->setBrush(QBrush(QColor(200, 220, 255)));
        node->setPen(QPen(Qt::black, 2));
        node->setFlag(QGraphicsItem::ItemIsMovable, true);
        node->setFlag(QGraphicsItem::ItemIsSelectable, true);
        node->setData(0, nodeId);

        QGraphicsTextItem* text = new QGraphicsTextItem(QString::number(nodeId+1), node);
        text->setDefaultTextColor(Qt::black);
        text->setFont(QFont("Arial", 12, QFont::Bold));
        text->setPos(-10, -10);

        addItem(node);
        nodeItems[nodeId] = node;

        if (nodeId >= nextNodeId) {
            nextNodeId = nodeId + 2;
        }
    }

    for (const auto& arrowData : arrowsData) {
        auto fromIt = nodeItems.find(arrowData.node_1);
        auto toIt = nodeItems.find(arrowData.node_2);

        if (fromIt != nodeItems.end() && toIt != nodeItems.end()) {
            ::Arrow* arrow = new ::Arrow(fromIt->second, toIt->second);

            arrow->setDouble(arrowData.isLoop);

            QString weightStr = QString::fromStdString(arrowData.weight);
            if (weightStr.contains(",")) {
                arrow->setFullWeight(weightStr); 
                int firstWeight = weightStr.split(",").first().toInt();
                arrow->setWeight(firstWeight);
            }
            else if (!weightStr.isEmpty()) {
                arrow->setFullWeight(weightStr);
                arrow->setWeight(weightStr.toInt());
            }
            else {
                arrow->setWeight(1);
            }

            arrows.append(arrow);
            addItem(arrow);
        }
    }
    updateAllArrows();
}

void DiagramScene::drawWays(const QStringList& ways)
{
    clearColors();

    for (const QString& way : ways) {
        QStringList parts = way.split("-", Qt::SkipEmptyParts);
        QVector<int> nodes_in_way;

        for (QString& part : parts) {
            nodes_in_way.append(part.toInt() - 1);
        }

        QVector<QPair<int, int>> arrows_in_way;
        for (int i = 0; i < nodes_in_way.size() - 1; i++) {
            arrows_in_way.append({ nodes_in_way[i], nodes_in_way[i + 1] });
        }
        for (Arrow* arrow : arrows) {
            int startId = arrow->startItem()->data(0).toInt();
            int endId = arrow->endItem()->data(0).toInt();

            for (const auto& pathArrow : arrows_in_way) {
                if (startId == pathArrow.first && endId == pathArrow.second) {
                    arrow->setGreat(true);
                    break;
                }
            }
        }
    }
    update();
}

void DiagramScene::clearColors() {
    for (Arrow* arrow : arrows) {
        arrow->setGreat(false);
    }
    update();
}