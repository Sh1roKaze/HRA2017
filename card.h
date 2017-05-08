/// \file card.h
/// \author xverny00
/// \brief Declaration of custom variant of QObject and QGraphicsPixmapItem, represents a single card in gui

#ifndef CARD_H
#define CARD_H

#include "objects.h"

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>

class MyCard : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:

    /// \brief Constructs a card
    /// 
    /// \param col Represents collumn in which card lays.
    /// \param parent Pointer to a parent object.
    explicit MyCard(int col = 0, QObject *parent = 0);
    
    /// \brief Sets the color and rank of card.
    ///
    /// \param hidden Whether or not is the card turned face up.
    /// \param color Color of the card.
    /// \param number Rand of the card.
    void setValue(int hidden, int color, int number);
    
    /// \brief If card is face down, turns it face up.
    void turnCard();
    
    /// \brief Gets collumn in which the card is.
    ///
    /// \return Collumn in which the card is.
    int getColumn();
    
    /// \brief Sets column in which the card is.
    ///
    /// \param target New collumn.
    void setColumn(int target);
    
    /// \brief Asks the game logic whether the move is valid.
    ///
    /// \param source Source card collumn.
    /// \param target Target card collumn.
    /// \param count How many cards were moved.
    /// \return 0 if move is valid, 16 if game is finished
    int isValidMove(int source, int target, int count);
    
    /// \brief Recursively moves cards by set amount.
    ///
    /// \param vert Vertical distance.
    /// \param hor Horizontal distance.
    void moveMyCard(qreal vert, qreal hor);
    
    /// \brief Recursively sets draw order of the cards
    ///
    /// \param z Draw order, increments by one with each recursive call.
    void setDrawOrder(int z);

    /// \brief Mouse press event handler.
    ///
    /// \param event Stores info about the event.
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    
    /// \brief Mouse move event handler.
    ///
    /// \param event Stores info about the event.
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    
    /// \brief Mouse release event handler.
    ///
    /// \param event Stores info about the event.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    MyCard *prev = NULL; ///< Previous card pointer.
    MyCard *next = NULL; ///< Next card pointer.

signals:

public slots:

private:

    QPixmap *face; ///< Pointer to face picture of the card. 

    short column = 0; ///< Collumn in which the card is.
    short followMouse = 0; ///Whether or not the card will follow mouse movements.
    short faceUp = 0; ///< Whether or not the card is face up.
    short movable = 0; ///< Whether or not the card can be moved.
};

#include "myview.h"

#endif // CARD_H
