#ifndef CARD_H
#define CARD_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

class Card : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Card(QObject *parent = 0);
    void setValue(int val);
    void moveCard(qreal vert, qreal hor);
    void setDrawOrder(int z);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    Card *prev = NULL;
    Card *next = NULL;

signals:

public slots:

private:
    short value = 0;
    short followMouse = 0;
    short movable = 0;
};

#endif // CARD_H
