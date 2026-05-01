#pragma once

#include <QGraphicsLineItem>
#include <QPolygonF>

class QGraphicsEllipseItem;

// Класс представления связей между узлами
class Arrow : public QGraphicsLineItem {
public:
    // Конструктор класса
    Arrow(QGraphicsEllipseItem* start, QGraphicsEllipseItem* end);
    // Метод обновления координат стрелки после изменения координат узлов
    void updatePosition();
    // Метод получения начального узла связи
    QGraphicsEllipseItem* startItem() const { return current_start; }
    // Метод получения конечного узла связи
    QGraphicsEllipseItem* endItem() const { return current_end; }

    // Метод установки числового веса связи
    void setWeight(float w) { weight = w; }
    // Метод установки строкового представления веса связи
    void setFullWeight(const QString& w) { fullWeight = w; }
    // Метод получения числового веса связи
    float getWeight() const { return weight; }
    // Метод получения строкового представления веса связи
    QString getFullWeight() const { return fullWeight; }

    // Метод проверки, является ли связь двунаправленной
    void setDouble(bool loop) { isDouble = loop; update(); }
    // Метод получения флага двунаправленности
    bool getDouble() const { return isDouble; }

    // Метод проверки, является ли связь частью оптимального пути
    void setGreat(bool great) { isGreat = great; update(); }
    // Метод получения флага оптимальности связи
    bool getGreat() const { return isGreat; }

    // Метод проверки, является ли связь частью цикла
    void setCycle(bool cycle) { isCycle = cycle; update(); }
    // Метод получения флага цикличности
    bool getCycle() const { return isCycle; }

protected:
    // Метод рисования стрелки (инструмент для рисования, параметры объекта рисования, параметры для виджета)
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    // Метод, задающий видимые ограничения для стрелки
    QRectF boundingRect() const override;

private:
    QGraphicsEllipseItem* current_start; // Указатель на начальный узел
    QGraphicsEllipseItem* current_end; // Указатель на конечный узел
    QPolygonF arrow_head; // Полигон для конца стрелки

    float weight; // Числовое значение веса связи
    bool isDouble = false; // Флаг наличия двунаправленной связи
    bool isGreat = false; // Флаг принадлежности связи к кратчайшему пути
    bool isCycle = false; // Флаг принадлежности связи к циклу
    QString fullWeight; // Строковое представление веса связи
};
