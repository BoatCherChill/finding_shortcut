#pragma once

#include <QGraphicsLineItem>
#include <QPolygonF>

class QGraphicsEllipseItem;

class Arrow : public QGraphicsLineItem {
public:
    Arrow(QGraphicsEllipseItem* start, QGraphicsEllipseItem* end); // конструктор стрелки
    void updatePosition(); // функция обновления координат стрелки после изменения координат узлов
    QGraphicsEllipseItem* startItem() const { return current_start; }
    QGraphicsEllipseItem* endItem() const { return current_end; }

    void setWeight(float w) { weight = w; }
    void setFullWeight(const QString& w) { fullWeight = w; }
    float getWeight() const { return weight; }
    QString getFullWeight() const { return fullWeight; }

    void setDouble(bool loop) { isDouble = loop; update(); }
    bool getDouble() const { return isDouble; }

    void setGreat(bool great) { isGreat = great; update(); }
    bool getGreat() const { return isGreat; }

    void setCycle(bool cycle) { isCycle = cycle; update(); }
    bool getCycle() const { return isCycle; }

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override; // функция рисования стрелки (инструмент для рисования, параметры объекта рисования, параметры для виджета)
    QRectF boundingRect() const override; // функция, задающая ограничения для стрелки

private:
    QGraphicsEllipseItem* current_start; // указатель на начальный узел
    QGraphicsEllipseItem* current_end; // указатель на конечный узел
    QPolygonF arrow_head; // полигон для наконечника

    float weight;
    bool isDouble = false; // тдвойная стрелка
    bool isGreat = false; // стрелка - часть лучшего пути
    bool isCycle = false;
    
    QString fullWeight;

    
};
