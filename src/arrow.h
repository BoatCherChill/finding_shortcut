#pragma once

#include <QGraphicsLineItem>
#include <QPolygonF>

class QGraphicsEllipseItem;

// Класс представления связей между узлами
class Arrow : public QGraphicsLineItem {
public:
    Arrow(QGraphicsEllipseItem* start, QGraphicsEllipseItem* end); // Конструктор класса (начальный узел, конечный узел)
    void updatePosition(); // Метод обновления координат стрелки после изменения координат узлов
    QGraphicsEllipseItem* startItem() const { return current_start; } // Метод получения начального узла связи
    QGraphicsEllipseItem* endItem() const { return current_end; } // Метод получения конечного узла связи

    void setWeight(float w) { weight = w; } // Метод установки числового веса связи (числовой вес)
    void setFullWeight(const QString& w) { fullWeight = w; } // Метод установки строкового представления веса связи (строковый вес)
    float getWeight() const { return weight; } // Метод получения числового веса связи
    QString getFullWeight() const { return fullWeight; } // Метод получения строкового представления веса связи

    void setDouble(bool loop) { isDouble = loop; update(); } // Метод установки флага двунаправленности (флаг)
    bool getDouble() const { return isDouble; } // Метод получения флага двунаправленности

    void setGreat(bool great) { isGreat = great; update(); } // Метод установки флага оптимальности пути (флаг)
    bool getGreat() const { return isGreat; } // Метод получения флага оптимальности пути

    void setCycle(bool cycle) { isCycle = cycle; update(); } // Метод установки флага цикличности (флаг)
    bool getCycle() const { return isCycle; } // Метод получения флага цикличности

protected:
    // Метод рисования стрелки (инструмент для рисования, параметры объекта рисования, параметры для виджета)
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QRectF boundingRect() const override; // Метод, задающий видимые ограничения для стрелки

private:
    QGraphicsEllipseItem* current_start; // Указатель на начальный узел
    QGraphicsEllipseItem* current_end; // Указатель на конечный узел
    QPolygonF arrow_head; // Полигон для наконечника стрелки

    float weight; // Числовое значение веса связи
    bool isDouble = false; // Флаг наличия двунаправленной связи
    bool isGreat = false; // Флаг принадлежности связи к кратчайшему пути
    bool isCycle = false; // Флаг принадлежности связи к циклу
    QString fullWeight; // Строковое представление веса связи
};