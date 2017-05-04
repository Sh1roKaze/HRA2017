#include "mywidget.h"

#include <QDebug>

myWidget::myWidget()
{
    /* layout init */
    viewList.append(new MyView());
    connect(viewList[0]->getButton5(), SIGNAL(buttonPressed()), this, SLOT(addGameButtonPressed()));
    connect(viewList[0], SIGNAL(stopButtonPressed()), this, SLOT(endGameButtonPressed()));

    empty1 = new QGraphicsView();
    empty1->setMinimumSize(640, 480);
    empty2 = new QGraphicsView();
    empty2->setMinimumSize(640, 480);

    fourGameLayout = new QGridLayout();
    fourGameLayout->setContentsMargins(0,0,0,0);
    fourGameLayout->setHorizontalSpacing(0);
    fourGameLayout->setVerticalSpacing(0);
    fourGameLayout->addWidget(viewList[0], 0, 0, 1, 1, 0);
    widget1 = new QWidget();
    widget1->setLayout(fourGameLayout);
    this->addWidget(widget1);

    /* sets active layout */
    this->setCurrentWidget(widget1);
}

void myWidget::applyViewList()
{
    /* removes finished games */
    for (int i = 0; i < viewList.length(); i++) {
        short gs = viewList[i]->getGameState();
        if (gs == 0) {
                fourGameLayout->removeWidget(viewList[i]);
                delete viewList[i];
                viewList.removeAt(i);
        }
    }

    /* sets layout */
    fourGameLayout->addWidget(viewList[0], 0, 0, 1, 1, 0);

    if (viewList.length() > 1) {
        fourGameLayout->addWidget(viewList[1], 0, 1, 1, 1, 0);
        fourGameLayout->addWidget(empty1, 1, 0, 1, 1, 0);
        fourGameLayout->addWidget(empty2, 1, 1, 1, 1, 0);
        empty1->show();
        empty2->show();
    } else {
        fourGameLayout->addWidget(viewList[0], 0, 0, 2, 2, 0);
        fourGameLayout->removeWidget(empty1);
        fourGameLayout->removeWidget(empty2);
        empty1->hide();
        empty2->hide();
    }

    if (viewList.length() == 3) {
        fourGameLayout->addWidget(viewList[2], 1, 0, 1, 1, 0);
    }

    if (viewList.length() == 4) {
        fourGameLayout->addWidget(viewList[3], 1, 1, 1, 1, 0);
    }
}

void myWidget::addGameButtonPressed()
{
    /* add game */
    if (runningGames < 4) {
        viewList.append(new MyView());
        MyView *temp = viewList.last();
        connect(temp->getButton5(), SIGNAL(buttonPressed()), this, SLOT(addGameButtonPressed()));
        connect(temp, SIGNAL(stopButtonPressed()), this, SLOT(endGameButtonPressed()));
        runningGames++;
    }
    applyViewList();
}

void myWidget::endGameButtonPressed()
{
    /* stop game */
    runningGames--;
    if (runningGames == 0) {
        viewList.append(new MyView());
        MyView *temp = viewList.last();
        connect(temp->getButton5(), SIGNAL(buttonPressed()), this, SLOT(addGameButtonPressed()));
        connect(temp, SIGNAL(stopButtonPressed()), this, SLOT(endGameButtonPressed()));
        runningGames++;
    }
    applyViewList();
}
