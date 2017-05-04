#include "card.h"

#include <QDebug>

short lastX;
short lastY;

Card::Card(QObject *parent) : QObject(parent)
{
    this->setPixmap(QPixmap(":/img/Resources/Cover.png"));
    this->setScale(0.35);
    this->setPos(0, 0);
    prev = NULL;
    next = NULL;
}

void Card::setValue(int val)
{
    if (14 > val and val > 0) {
        this->value = val;
        this->movable = 1;
        /*QString myStr = new QString(":/img/Resources");
        switch (card.getColor()) {
            case 0: myStr.append("hearts_"); break;
            case 1: myStr.append("diamonds_"); break;
            case 2: myStr.append("clubs_"); break;
            case 3: myStr.append("spades_"); break;
        }
        myStr.append(card.getNumber().toStr());
        myStr.append(".png"); */
        this->setPixmap(QPixmap(":/img/Resources/spades_A.png"));
    }
}

void Card::moveCard(qreal hor, qreal vert)
{
    if (x() + hor*scale() < 0 or x() + hor*scale() + pixmap().width()*scale() > 640) {
        return;
    }
    if (y() + vert*scale() < 0 or y() + vert*scale() + pixmap().height()*scale() > 480) {
        return;
    }
    this->setPos(x() + hor*scale(), y() + vert*scale());

    if (this->next != NULL) {
        this->next->moveCard(hor, vert);
    }
}

void Card::setDrawOrder(int z)
{
    setZValue(z);
    if (next != NULL)
        next->setDrawOrder(++z);
}

void Card::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton and this->movable == 1) {
        this->followMouse = 1;
        setDrawOrder(900);
        lastX = pos().x();
        lastY = pos().y();
    }
    if (event->button() == Qt::RightButton) {
        //setPixmap(QPixmap(":/img/Resources/spades_A.png"));
        this->movable = 1;
    }

}

void Card::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (this->followMouse == 1) {
        QPointF a = event->pos() - event->lastPos();
        moveCard(a.x(), a.y());
    }
}

void Card::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->followMouse = 0;
        if (movable == 1) {

            QList <QGraphicsItem*> list = this->collidingItems();
            Card *temp = this->next;
            while (temp != NULL) {
                int a = list.indexOf(temp);
                list.removeAt(a);
                temp = temp->next;
            }
            if (list.empty()) {
                moveCard((lastX - x())/this->scale(), (lastY - y())/this->scale());
            } else {
                //TODO consult game rules, connect to
            }
        }
    }
}
