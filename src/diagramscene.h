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

    DiagramScene(QMenu* itemMenu, QObject* parent = nullptr); // Конструктор класса (указатель на контекстное меню, родительский объект)

    void setMode(Mode mode); // Метод установки текущего режима работы (режим)
    Mode getMode() const { return my_mode; } // Метод получения текущего режима работы

    void deleteSelectedItem(); // Метод удаления выбранного элемента
    void clearAll(); // Метод очистки сцены

    void drawWays(const QStringList& ways); // Метод выделения связей на сцене (список путей в виде строк)
    void loadGraph(const vector<pair<int, pair<int, int>>>& nodesData, const vector<GraphArrow>& arrowsData); // Метод загрузки графа на сцену (список узлов, список связей)
    QVector<Arrow*> getArrows() const { return arrows; } // Метод получения всех связей графа на сцене

signals:
    void itemInserted(); // Сигнал о добавлении нового элемента на сцену
    void itemSelected(QGraphicsItem* item); // Сигнал выбора элемента на сцене (указатель на выбранный элемент)
    void graphChanged(int from, int to, Arrow* arrow); // Сигнал изменения структуры графа (ID начального узла, ID конечного узла, указатель на стрелку)

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override; // Метод обработки нажатия кнопки мыши (событие мыши)
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override; // Метод обработки перемещения мыши (событие мыши)
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override; // Метод обработки отпускания кнопки мыши (событие мыши)
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override; // Метод обработки двойного нажатия кнопки мыши (событие мыши)

private:
    void createNode(const QPointF& pos); // Метод создания нового узла (координаты позиции)
    void createArrow(QGraphicsEllipseItem* startNode, QGraphicsEllipseItem* endNode); // Метод создания новой связи (начальный узел, конечный узел)
    void updateAllArrows(); // Метод обновления положения связей при перемещении узлов
    void editArrowWeight(Arrow* arrow); // Метод изменения веса связи (указатель на стрелку)
    void clearColors(); // Метод сброса выделения всех связей
    
    Mode my_mode; // Текущий режим работы
    QMenu* my_menu; // Указатель на контекстное меню
    QGraphicsLineItem* tempLine; // Временная линия при создании связи
    QGraphicsEllipseItem* tempStartNode; // Узел, с которого начинается создание связи
    int nextNodeId; // Счетчик узлов
    QVector<Arrow*> arrows; // Список всех связей
};