#ifndef JTEXTITEM_H
#define JTEXTITEM_H

#include <QGraphicsTextItem>
#include <QGraphicsSceneMouseEvent>

class JTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    explicit JTextItem(QGraphicsItem *parent = nullptr);
    explicit JTextItem(const QString &text, QGraphicsItem *parent = nullptr);

protected Q_SLOTS:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
private:
    bool isDrag;
};

#endif // JTEXTITEM_H
