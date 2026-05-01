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

// Класс для графического отображения и редактирования графа
class DiagramScene : public QGraphicsScene {
    Q_OBJECT
public:
    // Режимы работы программы
    enum Mode { InsertNode, InsertArrow, EditItems };

    // Конструктор класса
    DiagramScene(QMenu* itemMenu, QObject* parent = nullptr);

    // Метод установки текущего режима работы
    void setMode(Mode mode);
    // Метод получения текущего режима работы
    Mode getMode() const { return my_mode; }

    // Метод удаления выбранного элемента
    void deleteSelectedItem();
    // Метод очистки сцены
    void clearAll();

    // Метод выделения связей на сцене
    void drawWays(const QStringList& ways);
    // Метод загрузки графа на сцену
    void loadGraph(const vector<pair<int, pair<int, int>>>& nodesData, const vector<GraphArrow>& arrowsData);
    // Метод получения всех связей графа на сцене
    QVector<Arrow*> getArrows() const { return arrows; }

signals:
    // Метод, сообщающий о добавлении нового элемента на сцену
    void itemInserted();
    // Метод, вызываемый при выборе элемента на сцене
    void itemSelected(QGraphicsItem* item);
    // Метод, реагирующий на изменение структуры графа
    void graphChanged(int from, int to, Arrow* arrow);

protected:
    // Метод обработки нажатия на кнопку мыши
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    // Метод обработки перемещения мыши
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    // Метод обработки удержания кнопки мыши
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    // Метод обработки двойного нажатия на кнопку мыши
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    // Метод создания нового узла
    void createNode(const QPointF& pos);
    // Метод создания новой связи
    void createArrow(QGraphicsEllipseItem* startNode, QGraphicsEllipseItem* endNode);
    // Метод обновления положения связей при перемещении узлов
    void updateAllArrows();
    // Метод изменения веса связи
    void editArrowWeight(Arrow* arrow);
    // Метод сбрасывания выделений всех связей
    void clearColors();
    
    Mode my_mode; // Текущий режим работы
    QMenu* my_menu; // Указатель на контекстное меню
    QGraphicsLineItem* tempLine; // Временная линия при создании связи
    QGraphicsEllipseItem* tempStartNode; // Узел, с которого начинается создание связи
    int nextNodeId; // Счетчик узлов
    QVector<Arrow*> arrows; // Список всех связей
};