#pragma once

// Отключаем Windows SDK conflict
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

// Отключаем std::byte
#define _HAS_STD_BYTE 0

#include <QGraphicsScene>
#include <QMenu>
#include <QVector>
#include <QString>
#include <vector>
#include "arrow.h"
#include "graph.h"

using namespace std;

struct GraphArrow;

class DiagramScene : public QGraphicsScene {
    Q_OBJECT
public:
    enum Mode { InsertNode, InsertArrow, EditItems };

    DiagramScene(QMenu* itemMenu, QObject* parent = nullptr);

    void setMode(Mode mode);
    Mode getMode() const { return my_mode; }

    void deleteSelectedItem();
    void clearAll();

    void drawWays(const QStringList& ways);

    void loadGraph(const vector<pair<int, pair<int, int>>>& nodesData, const vector<GraphArrow>& arrowsData);

    QVector<Arrow*> getArrows() const { return arrows; }

signals:
    void itemInserted();
    void itemSelected(QGraphicsItem* item);

    void graphChanged(int from, int to, Arrow* arrow);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void createNode(const QPointF& pos);
    void createArrow(QGraphicsEllipseItem* startNode, QGraphicsEllipseItem* endNode);
    void updateAllArrows();
    void editArrowWeight(Arrow* arrow);
    void clearColors();
    

    Mode my_mode;
    QMenu* my_menu;
    QGraphicsLineItem* tempLine;
    QGraphicsEllipseItem* tempStartNode;
    int nextNodeId;

    QVector<Arrow*> arrows;
};