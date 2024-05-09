#include "interactiveview.h"
#include <QWheelEvent>
#include <QKeyEvent>
#include <QDebug>

#define VIEW_CENTER viewport()->rect().center()
#define VIEW_WIDTH  viewport()->rect().width()
#define VIEW_HEIGHT viewport()->rect().height()

InteractiveView::InteractiveView(QWidget *parent)
	: QGraphicsView(parent),
	m_translateButton(Qt::LeftButton),
	m_translateSpeed(1.0),
	m_zoomDelta(0.1),
	m_bMouseTranslate(false),
	m_scale(1.0)
{
	// ȥ��������
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setCursor(Qt::PointingHandCursor);
	setRenderHint(QPainter::Antialiasing);

	setSceneRect(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX);
	centerOn(0, 0);
}

// ƽ���ٶ�
void InteractiveView::setTranslateSpeed(qreal speed)
{
	// �����ٶȷ�Χ
	Q_ASSERT_X(speed >= 0.0 && speed <= 2.0,
		"InteractiveView::setTranslateSpeed", "Speed should be in range [0.0, 2.0].");
	m_translateSpeed = speed;
}

qreal InteractiveView::translateSpeed() const
{
	return m_translateSpeed;
}

// ���ŵ�����
void InteractiveView::setZoomDelta(qreal delta)
{
	// ����������Χ
	Q_ASSERT_X(delta >= 0.0 && delta <= 1.0,
		"InteractiveView::setZoomDelta", "Delta should be in range [0.0, 1.0].");
	m_zoomDelta = delta;
	zoomIn();
}

void InteractiveView::setXOffet(qreal xof)
{
	XOffset = xof;
}

void InteractiveView::setYOffet(qreal yof)
{
	YOffset = yof;
}

qreal InteractiveView::zoomDelta() const
{
	return m_zoomDelta;
}

// ��/��/��/�Ҽ�����������ƶ�����/�����������š��ո�/�س�����ת
void InteractiveView::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Up:
		translate(QPointF(0, -2));  // ����
		break;
	case Qt::Key_Down:
		translate(QPointF(0, 2));  // ����
		break;
	case Qt::Key_Left:
		translate(QPointF(-2, 0));  // ����
		break;
	case Qt::Key_Right:
		translate(QPointF(2, 0));  // ����
		break;
	case Qt::Key_Plus:  // �Ŵ�
		zoomIn();
		break;
	case Qt::Key_Minus:  // ��С
		zoomOut();
		break;
	case Qt::Key_Space:  // ��ʱ����ת
		rotate(-5);
		break;
	case Qt::Key_Enter:  // ˳ʱ����ת
	case Qt::Key_Return:
		rotate(5);
		break;
	}
}

// ƽ��
void InteractiveView::mouseMoveEvent(QMouseEvent *event)
{
	if (m_bMouseTranslate) {
		QPointF mouseDelta = mapToScene(event->pos()) - mapToScene(m_lastMousePos);
		translate(mouseDelta);
	}

	m_lastMousePos = event->pos();
}

void InteractiveView::mousePressEvent(QMouseEvent *event)
{
	// ������ Item���������� item ʱͬ���϶�����
	m_bMouseTranslate = true;
	m_lastMousePos = event->pos();
	/*
	if (event->button() == m_translateButton) {
		// ��������û�� item ʱ�������ƶ�
		QPointF point = mapToScene(event->pos());
		if (scene()->itemAt(point, transform()) == nullptr)  {
			m_bMouseTranslate = true;
			m_lastMousePos = event->pos();
		}
	}
	*/
	if (event->button() == Qt::MidButton) {
		setSceneRect(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX);
		centerOn(0, 0);
		//        centerOn(sceneRect().height() - XOffset, sceneRect().height() - YOffset);
	}
}

void InteractiveView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == m_translateButton)
		m_bMouseTranslate = false;
}

// �Ŵ�/��С
void InteractiveView::wheelEvent(QWheelEvent *event)
{
	// ���ֵĹ�����
	QPoint scrollAmount = event->angleDelta();
	// ��ֵ��ʾ����Զ��ʹ���ߣ��Ŵ󣩣���ֵ��ʾ����ʹ���ߣ���С��
	scrollAmount.y() > 0 ? zoomIn() : zoomOut();
}

// �Ŵ�
void InteractiveView::zoomIn()
{
	zoom(static_cast<float>(1 + m_zoomDelta));
}

// ��С
void InteractiveView::zoomOut()
{
	zoom(static_cast<float>(1 - m_zoomDelta));
}

// ���� - scaleFactor�����ŵı�������
void InteractiveView::zoom(float scaleFactor)
{
	// ��ֹ��С�����
	qreal factor = transform().scale(static_cast<qreal>(scaleFactor), static_cast<qreal>(scaleFactor)).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.07 || factor > 100)
		return;

	scale(static_cast<qreal>(scaleFactor), static_cast<qreal>(scaleFactor));
	m_scale *= static_cast<qreal>(scaleFactor);
}

// ƽ��
void InteractiveView::translate(QPointF delta)
{
	// ���ݵ�ǰ zoom ����ƽ����
	delta *= m_scale;
	delta *= m_translateSpeed;

	// view ��������µĵ���Ϊê������λ scene
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	QPoint newCenter(static_cast<int>(VIEW_WIDTH / 2 - delta.x()), static_cast<int>(VIEW_HEIGHT / 2 - delta.y()));
	centerOn(mapToScene(newCenter));

	// scene �� view �����ĵ���Ϊê��
	setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}
