/// \file myview.h
/// \author xverny00
/// \brief Declaration of custom variant of QGraphicsView

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
#include <QMessageBox>

/// \brief Represents a single game.
///
/// Stores all the information about single game.
class MyView : public QGraphicsView
{
    Q_OBJECT

public:

    /// \brief Constructs a new game.
    MyView();
    
    /// \brief Loads game from game logic.
    void loadGame();
    
    /// \brief Overloads resizeEvent.
    ///
    /// \param event carries information about the resizeEvent.
    void resizeEvent(QResizeEvent *event);
    
    /// \brief Lays out cards in the view.
    ///
    /// \param scalingRatio Percentage of how the view height changed,value of 1 represents a width change, or no change at all. 
    void layoutCards(qreal scalingRatio);
    
    /// \brief Gets add game button pointer.
    ///
    /// \return Returns a pointer to button with index 5.
    MyButton *getButton5();
    
    /// \brief Used to get game state info.
    ///
    /// \return Game state, 0 means the game is finished 
    int getGameState();
    
    /// \brief Used to get pointer to game logic object.
    ///
    /// \return Returns a game logic object pointer.
    Hra2017::Game *getGameLogic();

public slots:

    /// \brief Remove button pressed event handler.
    void removeButtonPressed();
    
    /// \brief Load button pressed event hadler.
    void loadButtonPressed();
    
    /// \brief Load file selected event handler.
    ///
    /// \param QString Path to file.
    void loadSelected(QString);
    
    /// \brief Save button pressed event handler.
    void saveButtonPressed();
    
    /// \brief Save file selected event handler.
    ///
    /// \param QString Path to file.
    void saveSelected(QString);
    
    /// \brief Undo button pressed event handler.
    void undoButtonPressed();
    
    /// \brief Hint button pressed event handler.
    void hintButtonPressed();
    
    /// \brief Stock to foundation button pressed event handler.
    void stockToFoundation();

signals:
    /// \brief Stop button pressed event signal.
    void stopButtonPressed();

private:

    void softResetGame();  ///< Frees all the cards, used before loading.
    void resetGame(); ///< Frees everything, used to completely end the game.

    QGraphicsScene *scene; ///< Pointer to a graphics scene object
    MyCard *pile[7]; ///< Holds pointers to piles of cards.
    MyCard *foundation[4]; ///< Holds pointers to cards in foundation.
    MyButton *stock; ///< Pointer to stock to waste button.
    MyCard* waste; ///< Holds pointer to cards in waste.
    MyButton *button[6]; ///< Holds pointers to buttons.
    short gameState = 1; ///< Represents game state.
    Hra2017::Game *gameLogic; ///< Pointer to game logic object.
};

#endif // MYVIEW_H
