#ifndef MYVIEW_H
#define MYVIEW_H

#include "card.h"
#include "objects.h"
#include "mybutton.h"

#include <QObject>
#include <QLabel>
#include <QResizeEvent>
#include <QGraphicsView>
#include <QFileDialog>

class MyView : public QGraphicsView
{
    Q_OBJECT
public:
    MyView();
    void loadGame();
    void startNewGame();
    void resizeEvent(QResizeEvent *event);
    void layoutCards(qreal scalingRatio);
    MyButton *getButton5();
    int getGameState();

public slots:
    void removeButtonPressed();
    void loadButtonPressed();
    void loadSelected(QString);
    void saveButtonPressed();
    void saveSelected(QString);

signals:
    void stopButtonPressed();

private:
    void resetGame();

    QGraphicsScene *scene;
    QList <MyCard*> cards;
    MyCard *pile[7];
    MyCard *foundation[4];
    MyCard *stock;
    MyCard* waste;
    MyButton *button[6];
    short gameState = 1;
    Hra2017::Game *gameLogic;
};

#endif // MYVIEW_H
