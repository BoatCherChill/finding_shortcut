#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>
#include <queue>
#include <cctype>
#include <QString>
#include <QGraphicsEllipseItem>
#include <QVector>
#include <QString>

using namespace std;

struct Node {
	int nodeID;
	pair<int, int> coordinates;
	vector<int> prev_nodes;
	string result; // результат вычисления узла
	bool calculated; // флаг проведения вычислений
	Node() : nodeID(0), calculated(false) {}

};

class VisualNode : public QGraphicsEllipseItem {
public:
	VisualNode(int id, QGraphicsItem* parent = nullptr);
	int getNodeId() const { return nodeID; }
	void setNodeId(int id) { nodeID = id; }
	int getNextId() const { return next_NodeID; }
	VisualNode() : next_NodeID(0) {}

protected:
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override; // функция обработки двойного нажатия мыши
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override; // функция отрисовки узла
	QVariant itemChange(GraphicsItemChange change, const QVariant& value) override; // функция изменения позиции узла

private:
	int nodeID;
	bool calculated;
	double value;
	int next_NodeID; // следуюзий свободный ID
};
