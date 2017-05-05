#ifndef MYWIDGET_H
#define MYWIDGET_H

#include "card.h"
#include "objects.h"
#include "myview.h"

#include <QHBoxLayout>
#include <QGridLayout>
#include <QGraphicsView>
#include <QStackedWidget>

class myWidget : public QStackedWidget
{
    Q_OBJECT

public:
    myWidget();
    void applyViewList();

public slots:
    void addGameButtonPressed();
    void endGameButtonPressed();

private:
    short layoutSwitch = 0;
    short runningGames = 1;

    QWidget *widget0;
    QWidget *widget1;

    QGridLayout *fourGameLayout;
    QHBoxLayout *singleGameLayout;

    QList<MyView*> viewList;

    QGraphicsView *empty1;
    QGraphicsView *empty2;
};

#endif // MYWIDGET_H
