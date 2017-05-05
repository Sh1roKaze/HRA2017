#include "card.h"

#include <QDebug>
#include <string>

short lastX;
short lastY;

MyCard::MyCard(QObject *parent) : QObject(parent)
{
    this->setPixmap(QPixmap(":/img/Resources/Cover.png"));
    this->setScale(0.35);
    this->setPos(0, 0);
    prev = NULL;
    next = NULL;
}

void MyCard::setValue(int hidden, int color, int number)
{
        QString *myStr = new QString(":/img/Resources/");
        switch (color) {
            case 0: myStr->append("hearts_"); break;
            case 1: myStr->append("diamonds_"); break;
            case 2: myStr->append("clubs_"); break;
            case 3: myStr->append("spades_"); break;
        }
        myStr->append(QString::number(number));
        myStr->append(".png");
        qDebug() << *myStr;
        face = new QPixmap(*myStr);
        if (hidden) {
            setPixmap(*face);
        }
}

void MyCard::moveMyCard(qreal hor, qreal vert)
{
    this->setPos(x() + hor*scale(), y() + vert*scale());

    if (this->next != NULL) {
        this->next->moveMyCard(hor, vert);
    }
}

void MyCard::setDrawOrder(int z)
{
    setZValue(z);
    if (next != NULL)
        next->setDrawOrder(++z);
}

void MyCard::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton and this->movable == 1) {
        this->followMouse = 1;
        setDrawOrder(900);
        lastX = pos().x();
        lastY = pos().y();
    }
    if (event->button() == Qt::RightButton) {
        this->movable = 1;
    }

}

void MyCard::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (this->followMouse == 1) {
        QPointF a = event->pos() - event->lastPos();
        moveMyCard(a.x(), a.y());
    }
}

void MyCard::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->followMouse = 0;
        if (movable == 1) {

            QList <QGraphicsItem*> list = this->collidingItems();
            MyCard *temp = this->next;
            while (temp != NULL) {
                int a = list.indexOf(temp);
                list.removeAt(a);
                temp = temp->next;
            }
            if (list.empty()) {
                moveMyCard((lastX - x())/this->scale(), (lastY - y())/this->scale());
            } else {
                //TODO consult game rules, connect to
            }
        }
    }
}
