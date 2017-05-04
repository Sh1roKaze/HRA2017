#include "mybutton.h"

MyButton::MyButton(QObject *parent) : QObject(parent)
{
    setScale(0.20);
}

void MyButton::setURL(const char *str1)
{
    normal = QPixmap(str1);
    setPixmap(normal);
}

void MyButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit buttonPressed();
    }
}
