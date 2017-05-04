#ifndef MYVIEW_H
#define MYVIEW_H

#include "card.h"
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
    void startNewGame();
    void resizeEvent(QResizeEvent *event);
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
    QList <Card*> cards;
    Card *pile[7];
    Card *foundation[4];
    MyButton *button[6];
    short reset = 0;
    short gameState = 0;
};

#endif // MYVIEW_H
