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

// Конструктор класса (указатель на контекстное меню, родительский объект)
DiagramScene::DiagramScene(QMenu* itemMenu, QObject* parent)
    : QGraphicsScene(parent), my_menu(itemMenu), my_mode(InsertNode), tempLine(nullptr), tempStartNode(nullptr), nextNodeId(1)
{
    setSceneRect(-5000, -5000, 10000, 10000);
    setBackgroundBrush(Qt::white);
}

void DiagramScene::setMode(Mode mode) {
    my_mode = mode;
    bool movable = (mode != InsertArrow); // В режиме InsertArrow узлы нельзя двигать
    for (QGraphicsItem* item : items()) {
        if (QGraphicsEllipseItem* node = qgraphicsitem_cast<QGraphicsEllipseItem*>(item))
            node->setFlag(QGraphicsItem::ItemIsMovable, movable);
    }
}

// Метод создания нового узла (координаты позиции)
void DiagramScene::createNode(const QPointF& pos) {
    QGraphicsEllipseItem* node = new QGraphicsEllipseItem(-30, -30, 60, 60); // Создать эллипс
    node->setPos(pos); // Установить позицию
    node->setBrush(QBrush(QColor(200, 220, 255))); // Установить цвет заливки
    node->setPen(QPen(Qt::black, 2)); // Установить цвет и толщину контура
    node->setFlag(QGraphicsItem::ItemIsMovable, true); // Разрешить перемещение
    node->setFlag(QGraphicsItem::ItemIsSelectable, true); // Разрешить выделение
    node->setData(0, nextNodeId - 1); // Сохранить ID узла

    QGraphicsTextItem* text = new QGraphicsTextItem(QString::number(nextNodeId), node); // Создать текст с номером
    text->setDefaultTextColor(Qt::black); // Установить цвет текста
    text->setFont(QFont("Arial", 12, QFont::Bold)); // Установить шрифт
    text->setPos(-10, -10); // Установить позицию текста

    addItem(node); // Добавить узел на сцену
    nextNodeId++;
}

// Метод создания новой связи (начальный узел, конечный узел)
void DiagramScene::createArrow(QGraphicsEllipseItem* startNode, QGraphicsEllipseItem* endNode) {
    if (!startNode || !endNode || startNode == endNode) 
        return;
    // Проверить существует ли такая связь
    for (Arrow* a : arrows) {
        if (a->startItem() == startNode && a->endItem() == endNode || a->startItem() == endNode && a->endItem() == startNode)
            return;
    }

    Arrow* arrow = new Arrow(startNode, endNode); // Создать новую стрелку

    bool ok;
    float weight = QInputDialog::getDouble(nullptr, "Вес связи",
        "Введите вес (положительное число):", 1.0, 1.0, 1000.0, 3, &ok);
    if (!ok) {
        delete arrow;
        return;
    }
    // Установить вес
    arrow->setWeight(weight); 
    arrow->setFullWeight(QString::number(weight));

    arrows.append(arrow); // Добавить в список
    addItem(arrow); // Добавить на сцену

    int fromId = startNode->data(0).toInt(); // Получить ID начального узла
    int toId = endNode->data(0).toInt(); // Получить ID конечного узла
    emit graphChanged(fromId, toId, arrow); // Сигнал об изменении графа
}

// Метод изменения веса связи (указатель на стрелку)
void DiagramScene::editArrowWeight(Arrow* arrow) {
    if (!arrow) return;

    bool ok;
    float newWeight = QInputDialog::getDouble(nullptr, "Редактирование веса",
        "Введите новый вес:", arrow->getWeight(), 1.0, 1000.0, 3, &ok);
    if (ok) {
        arrow->setWeight(newWeight); // Установить новый вес
        arrow->setFullWeight(QString::number(newWeight)); // Обновить отображаемый вес
        int fromId = arrow->startItem()->data(0).toInt(); // Получить ID начального узла
        int toId = arrow->endItem()->data(0).toInt(); // Получить ID конечного узла
        emit graphChanged(fromId, toId, arrow); // Сигнал об изменении графа
        update(); // Обновить сцену
    }
}

// Метод обновления положения связей при перемещении узлов
void DiagramScene::updateAllArrows() {
    for (Arrow* arrow : arrows)
        arrow->updatePosition(); // Обновить позицию каждой стрелки
    update(); // Обновить сцену
}

// Метод удаления выбранного элемента
void DiagramScene::deleteSelectedItem() {
    QList<QGraphicsItem*> selected = selectedItems(); // Получить список выбранных элементов
    if (selected.isEmpty()) return; // Если ничего не выбрано, выйти

    for (QGraphicsItem* item : selected) {
        if (QGraphicsEllipseItem* node = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) { // Если удаляется узел
            QVector<Arrow*> arrowsToDelete;
            for (Arrow* arrow : arrows) { // Найти все стрелки, связанные с узлом
                if (arrow->startItem() == node || arrow->endItem() == node)
                    arrowsToDelete.append(arrow);
            }

            for (Arrow* arrow : arrowsToDelete) { // Удалить найденные стрелки
                arrows.removeOne(arrow);
                removeItem(arrow);
                delete arrow;
            }

            removeItem(node); // Удалить узел
            delete node;
        }
        else if (Arrow* arrow = qgraphicsitem_cast<Arrow*>(item)) { // Если удаляется стрелка
            arrows.removeOne(arrow); // Удалить из списка
            removeItem(arrow); // Удалить со сцены
            delete arrow;
        }
    }

    emit graphChanged(0, 0, nullptr); // Сигнал об изменении графа
    update(); // Обновить сцену
}

// Метод обработки нажатия на кнопку мыши (событие мыши)
void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() != Qt::LeftButton) { // Если нажата не левая кнопка
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    if (my_mode == InsertNode) // Режим добавления узлов
        createNode(event->scenePos()); // Создать узел в позиции клика

    else if (my_mode == InsertArrow) { // Режим добавления связей
        QList<QGraphicsItem*> itemsAtPos = items(event->scenePos()); // Найти элементы под курсором
        for (QGraphicsItem* item : itemsAtPos) {
            if (QGraphicsEllipseItem* node = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {
                tempStartNode = node; // Запомнить начальный узел
                tempLine = new QGraphicsLineItem(QLineF(event->scenePos(), event->scenePos())); // Создать временную линию
                tempLine->setPen(QPen(Qt::red, 2, Qt::DashLine)); // Установить стиль линии
                tempLine->setZValue(1000); // Установить высокий Z-порядок
                addItem(tempLine); // Добавить линию на сцену
                break;
            }
        }
    }
    else if (my_mode == EditItems) { // Режим редактирования
        QGraphicsScene::mousePressEvent(event); // Стандартная обработка

        QList<QGraphicsItem*> itemsAtPos = items(event->scenePos()); // Найти элементы под курсором
        for (QGraphicsItem* item : itemsAtPos) {
            if (Arrow* arrow = qgraphicsitem_cast<Arrow*>(item)) {
                emit itemSelected(arrow); // Сигнал выбора стрелки
                break;
            }
        }
    }
}

// Метод обработки двойного нажатия на кнопку мыши (событие мыши)
void DiagramScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    if (my_mode == EditItems) { // В режиме редактирования
        QList<QGraphicsItem*> itemsAtPos = items(event->scenePos()); // Найти элементы под курсором
        for (QGraphicsItem* item : itemsAtPos) {
            if (Arrow* arrow = qgraphicsitem_cast<Arrow*>(item)) {
                editArrowWeight(arrow); // Редактировать вес стрелки
                break;
            }
        }
    }

    QGraphicsScene::mouseDoubleClickEvent(event); // Стандартная обработка
}

// Метод обработки перемещения мыши (событие мыши)
void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (tempLine && tempStartNode)
        tempLine->setLine(QLineF(tempStartNode->pos(), event->scenePos())); // Обновить временную линию
    
    updateAllArrows(); // Обновить положение стрелок
    QGraphicsScene::mouseMoveEvent(event); // Стандартная обработка
}


// Метод обработки удержания кнопки мыши (событие мыши)
void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (tempLine && tempStartNode) {
        QList<QGraphicsItem*> itemsAtPos = items(event->scenePos()); // Найти элементы под курсором
        QGraphicsEllipseItem* endNode = nullptr;

        for (QGraphicsItem* item : itemsAtPos) {
            if (QGraphicsEllipseItem* node = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {
                if (node != tempStartNode) { // Найден конечный узел, отличный от начального
                    endNode = node;
                    break;
                }
            }
        }

        removeItem(tempLine); // Удалить временную линию
        delete tempLine;
        tempLine = nullptr;

        if (endNode)
            createArrow(tempStartNode, endNode); // Создать стрелку

        tempStartNode = nullptr; // Сбросить начальный узел
    }

    QGraphicsScene::mouseReleaseEvent(event); // Стандартная обработка
}

// Метод очистки сцены
void DiagramScene::clearAll() {
    for (Arrow* arrow : arrows) { // Удалить все стрелки
        removeItem(arrow);
        delete arrow;
    }
    arrows.clear();

    QList<QGraphicsItem*> itemsToDelete;
    for (QGraphicsItem* item : items()) { // Найти все узлы на сцене
        if (QGraphicsEllipseItem* node = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {
            itemsToDelete.append(item);
        }
    }

    for (QGraphicsItem* item : itemsToDelete) { // Удалить все узлы
        removeItem(item);
        delete item;
    }

    nextNodeId = 1; // Сбросить счетчик узлов
}

// Метод загрузки графа на сцену (список узлов, список связей)
void DiagramScene::loadGraph(const vector<pair<int, pair<int, int>>>& nodesData, const vector<GraphArrow>& arrowsData) {
    clearAll(); // Очистить сцену

    map<int, QGraphicsEllipseItem*> nodeItems; // Словарь для соответствия ID -> узел

    // Создать узлы
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

        QGraphicsTextItem* text = new QGraphicsTextItem(QString::number(nodeId + 1), node);
        text->setDefaultTextColor(Qt::black);
        text->setFont(QFont("Arial", 12, QFont::Bold));
        text->setPos(-10, -10);

        addItem(node);
        nodeItems[nodeId] = node;

        if (nodeId >= nextNodeId)
            nextNodeId = nodeId + 2; // Обновить счетчик узлов
    }

    for (const auto& arrowData : arrowsData) { // Создать стрелки
        auto fromIt = nodeItems.find(arrowData.node_1);
        auto toIt = nodeItems.find(arrowData.node_2);

        if (fromIt != nodeItems.end() && toIt != nodeItems.end()) {
            ::Arrow* arrow = new ::Arrow(fromIt->second, toIt->second);

            arrow->setDouble(arrowData.isLoop); // Установить флаг двойной связи

            QString weightStr = QString::fromStdString(arrowData.weight);
            if (weightStr.contains(",")) { // Если вес содержит запятую (двойная связь)
                arrow->setFullWeight(weightStr);
                int firstWeight = weightStr.split(",").first().toInt();
                arrow->setWeight(firstWeight);
            }
            else if (!weightStr.isEmpty()) { // Обычный вес
                arrow->setFullWeight(weightStr);
                arrow->setWeight(weightStr.toInt());
            }
            else { // Вес по умолчанию
                arrow->setWeight(1);
            }

            arrows.append(arrow);
            addItem(arrow);
        }
    }
    updateAllArrows(); // Обновить положение стрелок
}

// Метод выделения связей на сцене (список путей в виде строк)
void DiagramScene::drawWays(const QStringList& ways) {
    clearColors(); // Сбросить предыдущие выделения

    for (const QString& way : ways) {
        QStringList parts = way.split("-", Qt::SkipEmptyParts); // Разбить путь на узлы
        QVector<int> nodes_in_way;

        for (QString& part : parts) {
            nodes_in_way.append(part.toInt() - 1); // Преобразовать в ID узла
        }

        QVector<QPair<int, int>> arrows_in_way;
        for (int i = 0; i < nodes_in_way.size() - 1; i++) { // Собрать пары узлов для стрелок
            arrows_in_way.append({ nodes_in_way[i], nodes_in_way[i + 1] });
        }

        for (Arrow* arrow : arrows) { // Найти и выделить нужные стрелки
            int startId = arrow->startItem()->data(0).toInt();
            int endId = arrow->endItem()->data(0).toInt();

            for (const auto& pathArrow : arrows_in_way) {
                if (startId == pathArrow.first && endId == pathArrow.second) {
                    arrow->setGreat(true); // Выделить стрелку (зеленым цветом)
                    break;
                }
            }
        }
    }
    update(); // Обновить сцену
}

// Метод сбрасывания выделений всех связей
void DiagramScene::clearColors() {
    for (Arrow* arrow : arrows) {
        arrow->setGreat(false); // Снять выделение со всех стрелок
    }
    update(); // Обновить сцену
}