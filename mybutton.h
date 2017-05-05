#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

class MyButton : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit MyButton(QObject *parent = 0);
    void setURL(const char *str1);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

signals:
    void buttonPressed();

public slots:

private:
    QPixmap normal;
};

#endif // MYBUTTON_H
