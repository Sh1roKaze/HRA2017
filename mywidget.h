/// \file mywidget.h
/// \author xverny00
/// \brief Declaration of custom variant of QStackedWidget

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

    /// \brief Constructs a widget.
    myWidget();
    
    /// \brief Lays out views on the screen.
    void applyViewList();

public slots:

    /// \brief Add game button pressed event handler.
    void addGameButtonPressed();
    
    /// \brief Stop game button pressed event handler.
    void endGameButtonPressed();

private:

    short runningGames = 1; ///< Number of running games.

    QWidget *widget0;
    QWidget *widget1;

    QGridLayout *fourGameLayout;
    QHBoxLayout *singleGameLayout;

    QList<MyView*> viewList;

    QGraphicsView *empty1;
    QGraphicsView *empty2;
};

#endif // MYWIDGET_H
