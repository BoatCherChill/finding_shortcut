#include "arrow.h"
#include <QPainter>
#include <QtMath>
#include <QGraphicsScene>

//конструктор стрелки (начальный узел, конечный узел)
Arrow::Arrow(QGraphicsEllipseItem* start, QGraphicsEllipseItem* end) : current_start(start), current_end(end) {
    setFlag(QGraphicsItem::ItemIsSelectable, true); // разрешить выделение стрелки
    setPen(QPen(Qt::black, 2));
}

// функция вычисления области перерисовки стрелки
QRectF Arrow::boundingRect() const {
    qreal extra = (pen().width() + 20) / 2.0; // дополнительный запас для наконечника
    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(), line().p2().y() - line().p1().y()))
        .normalized() // модуль
        .adjusted(-extra, -extra, extra, extra);
}

// функция отрисовки стрелки (инструмент для рисования, параметры объекта рисования, параметры для виджета)
void Arrow::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    // настроить перо
    QPen myPen = pen();
    if (isLoop || isCycle)
        myPen.setColor(Qt::red);
    else if (isGreat)
        myPen.setColor(Qt::green);
    else
        myPen.setColor(Qt::black);

    painter->setPen(myPen);

    // вычислить координаты начального и конечного узлов
    QPointF start_position = current_start->pos();
    QPointF end_position = current_end->pos();

    // вычислить направления и длины
    double delta_x = end_position.x() - start_position.x();
    double delta_y = end_position.y() - start_position.y();
    double length = sqrt(delta_x * delta_x + delta_y * delta_y);

    if (length < 0.1)
        return;

    // Вычислить нормализованное направление и перпендикуляр
    double dirX = delta_x / length;
    double dirY = delta_y / length;
    double perpX = -dirY;
    double perpY = dirX;

    double offset = 35; // Отступ от центра узла

    // нарисовать точки начала и конца линии с отступами от центров
    QPointF start_point(start_position.x() + dirX * offset, start_position.y() + dirY * offset);
    QPointF end_point(end_position.x() - dirX * offset, end_position.y() - dirY * offset);

    painter->drawLine(start_point, end_point); // нарисовать линию между точками

    // Нарисовать наконечник
    if (!isLoop) {
        qreal arrow_size = 15;
        QPointF arrow_left = end_point - QPointF(dirX * arrow_size - perpX * arrow_size / 2, dirY * arrow_size - perpY * arrow_size / 2);
        QPointF arrow_right = end_point - QPointF(dirX * arrow_size + perpX * arrow_size / 2, dirY * arrow_size + perpY * arrow_size / 2);

        arrow_head.clear();
        arrow_head << end_point << arrow_left << arrow_right;
        painter->drawPolygon(arrow_head);
    }

    double textOffset = 15;
    QPointF virtualStart = start_point - QPointF(perpX * textOffset, perpY * textOffset);
    QPointF virtualEnd = end_point - QPointF(perpX * textOffset, perpY * textOffset);

   
    QPointF mid_point = (virtualStart + virtualEnd) / 2;

    
    painter->setPen(QPen(Qt::blue, 2));
    painter->setFont(QFont("Arial", 10, QFont::Bold));
    painter->drawText(mid_point, fullWeight.isEmpty() ? QString::number(weight) : fullWeight);

    // Если стрелка выделена, рисуем пунктирные линии выделения
    if (isSelected()) {
        painter->save();
        painter->setPen(QPen(Qt::red, 1, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);

        QLineF lineUp = QLineF(start_point, end_point);
        lineUp.translate(perpX * 8, perpY * 8);
        painter->drawLine(lineUp);

        QLineF lineDown = QLineF(start_point, end_point);
        lineDown.translate(-perpX * 8, -perpY * 8);
        painter->drawLine(lineDown);

        painter->restore();
    }
}

// функция обновления позиции стрелки при перемещении узлов
void Arrow::updatePosition() {
    if (!current_start || !current_end)
        return;
    QLineF line(mapFromItem(current_start, 0, 0), mapFromItem(current_end, 0, 0)); // пересчитать положение в локальных координатах (относительно стрелки)
    setLine(line);
    update();
}
