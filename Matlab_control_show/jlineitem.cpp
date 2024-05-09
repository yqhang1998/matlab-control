#include "jlineitem.h"

JLineItem::JLineItem(const QLineF &line, const QPen &pen)
	: m_line(line)
	, m_pen(pen)
{
	init();
}

JLineItem::JLineItem(const QPointF &p1, const QPointF &p2, const QPen &pen)
	: m_line(p1, p2)
	, m_pen(pen)
{
	init();
}

JLineItem::JLineItem(qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen)
	: m_line(x1, y1, x2, y2)
	, m_pen(pen)
{
	init();
}

void JLineItem::init()
{
	calcuateRect();
}

QRectF JLineItem::boundingRect() const
{
	return m_rectf;
}

QPainterPath JLineItem::shape() const
{
	QPainterPath path;
	path.addRect(m_rectf);
	return path;
}

void JLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option)
		Q_UNUSED(widget)
		painter->setClipRect(option->exposedRect);
	painter->setPen(m_pen);
	painter->drawLine(m_line);
}

void JLineItem::updateLines()
{
	update();
}

void JLineItem::calcuateRect()
{
	// 计算画线所占矩形坐标
	m_rectf.setWidth(fabs(m_line.x2() - m_line.x1()));
	m_rectf.setHeight(fabs(m_line.y2() - m_line.y1()));
}

void JLineItem::setPen(const QPen &pen)
{
	m_pen = pen;
}

QPen JLineItem::pen() const
{
	return m_pen;
}

void JLineItem::setColor(const QColor &color)
{
	m_pen.setColor(color);
}

QColor JLineItem::color() const
{
	return m_pen.color();
}

void JLineItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mousePressEvent(event);
	update();
}

void JLineItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseMoveEvent(event);
	update();
}

void JLineItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseReleaseEvent(event);
	update();
}
