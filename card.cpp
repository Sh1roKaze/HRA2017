#include "card.h"

#include <QDebug>
#include <string>

short lastX;
short lastY;

MyCard::MyCard(int col, QObject *parent) : QObject(parent)
{
    this->setPixmap(QPixmap(":/img/Resources/Cover.png"));
    this->setScale(0.35);
    column = col;
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
        face = new QPixmap(*myStr);
        if (!hidden) {
            setPixmap(*face);
            movable = 1;
        }
}

void MyCard::turnCard()
{
    if (movable != 1) {
        setPixmap(*face);
        movable = 1;
    }
}

int MyCard::getColumn()
{
    return column;
}

int MyCard::isValidMove(int source, int target, int count)
{
    MyView* thisView = (MyView*) this->parent();
    Hra2017::Game *hra = thisView->getGameLogic();

    if (source == target) { return 0; }
    if (source < 0 or source > 11) { return 0; }
    if (target < 1 or target > 11) { return 0; }

    if (source == 0) {
        if (target < 8) {
            qDebug() << "Waste to tableu" << source << target-1;
            return hra->moveFromWasteToTableau(target-1);
        }
        qDebug() << "Waste to found" << source << target-8;
        return hra->moveFromWasteToFoundation(target-8);
    }

    if (source < 8) {
        if (target < 8) {
            qDebug() << "Tableu to tableu" << source-1 << target-1 << count;
            return hra->moveFromTableauToTableau(source - 1, target-1, count);
        }
        qDebug() << "Tabelu to found" << source-1 << target-8;
        return hra->moveFromTableauToFoundation(source - 1, target - 8);
    }

    if (source < 12) {
        if (target < 8) {
            qDebug() << "Found to tableu" << source-8 << target-1;
            hra->moveFromFoundationToTableau(source - 8, target - 1);
        }
        return 0;
    }
    return 0;
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
                int target =  ((MyCard*) list.last())->getColumn();
                //TODO consult game rules, connect to
                qDebug() << "Source column: " << column;
                qDebug() << "Target column: " << target;
                MyCard *temp = next;
                int count = 1;
                while (temp != NULL) {
                    count++;
                    temp =temp->next;
                }
                if (isValidMove(column, target, count)) {
                    //connectMyCard();
                    qDebug() << "Valid move";
                } else {
                    moveMyCard((lastX - x())/this->scale(), (lastY - y())/this->scale());
                }
            }
        }
    }
}
