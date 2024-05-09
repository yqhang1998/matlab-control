#ifndef JLINEITEM_H
#define JLINEITEM_H

#include <QGraphicsItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class JLineItem : public QGraphicsItem
{
public:
    explicit JLineItem(const QLineF &line, const QPen &pen = QPen());
    explicit JLineItem(const QPointF &p1, const QPointF &p2, const QPen &pen = QPen());
    explicit JLineItem(qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen = QPen());

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

    void updateLines();

    void calcuateRect();

    void setPen(const QPen &pen);
    QPen pen() const;

    void setColor(const QColor &color);
    QColor color() const;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
private:
    void init();

private:
    QLineF m_line;
    QColor m_color;
    QRectF m_rectf;
    QPen m_pen;
};

#endif // JLINEITEM_H
