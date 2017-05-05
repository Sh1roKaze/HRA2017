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
    explicit MyCard(int col = 0, QObject *parent = 0);
    void setValue(int hidden, int color, int number);
    void turnCard();
    int getColumn();
    int isValidMove(int source, int target, int count);
    void connectMyCard();
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
    QPixmap *face;

    short column = 0;
    short followMouse = 0;
    short faceUp = 0;
    short movable = 0;
};

#include "myview.h"

#endif // CARD_H
