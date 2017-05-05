#ifndef CARD_H
#define CARD_H

#include "objects.h"

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

class MyCard : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit MyCard(QObject *parent = 0);
    void setValue(int color, int number);
    void moveMyCard(qreal vert, qreal hor);
    void setDrawOrder(int z);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    MyCard *prev = NULL;
    MyCard *next = NULL;

signals:

public slots:

private:
    short value = 0;
    short followMouse = 0;
    short movable = 0;
};

#endif // CARD_H
