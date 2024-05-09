#include "jtextitem.h"

JTextItem::JTextItem(QGraphicsItem *parent)
	: QGraphicsTextItem(parent)
	, isDrag(false)
{

}

JTextItem::JTextItem(const QString &text, QGraphicsItem *parent)
	: QGraphicsTextItem(text, parent)
	, isDrag(false)
{

}

void JTextItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	/*
	if(shape().contains(event->pos()))
	 {
		 QGraphicsItem::mousePressEvent(event);
		 isDrag = true;
	 }
	 else
		event->ignore();
	*/
	event->ignore();
}

void JTextItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	/*
	if(isDrag)
		QGraphicsItem::mouseMoveEvent(event);
	else
		event->ignore();
	*/
	event->ignore();
}

void JTextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	/*
	if(isDrag)
		QGraphicsItem::mouseMoveEvent(event);
	else
		event->ignore();
	isDrag = false;
	*/
	event->ignore();
}
