// ICP Project
// Objects representing state of game
// xvlach16

#ifndef OBJECTS_H
#define OBJECTS_H

#include <vector>
#include <stack>
#include <algorithm>
#include <string>

namespace Hra2017
{
    /// \brief Represents rank of card.
    enum CardNumber
    {
        none,
        ace,
        two,
        three,
        four,
        five,
        six,
        seven,
        eight,
        nine,
        ten,
        jack,
        queen,
        king
    };

    /// \brief Represents color of card.
    enum CardColor
    {
        hearts = 0,
        diamonds = 1, tiles = 1,
        clubs = 2, clovers = 2,
        spades = 3, pikes = 3
    };

    /// \brief Represents one specific pile of cards on play ground.
    enum Pile
    {
        waste,
        tableau0,
        tableau1,
        tableau2,
        tableau3,
        tableau4,
        tableau5,
        tableau6,
        foundation0,
        foundation1,
        foundation2,
        foundation3
    };

    /// \brief Represents basic information about card.
    struct CardInfo
    {
        CardNumber number{none}; ///< Represents rank of card.
        CardColor color{hearts}; ///< Represents color of card.
        bool hidden{false}; ///< If hidden is true, back side of card is visible, otherwise the front side.
    };
    
    class Card;
    /// \brief Represents one move to be saved into history of moves (for case of undo).
    struct Move
    {
        Card *card; ///< The card which was moved.
        Card **from; ///< Base where was card moved from.
        int score; ///< Score before the move.
    };

    /// \brief Represents recommended move.
    struct Hint
    {
        Pile source{waste}; ///< Source pile where card(s) could be moved from.
        Pile destination{waste}; ///< Destination pile where cards could be moved.
        int number{0}; ///< Number of cards to be moved.

        Hint() { } ///< Initialises hint with default values.
        Hint(Pile s, Pile d, int n) : source(s), destination(d), number(n) { } ///< Initialises hint with specifed values.
        Hint(int s, int d, int n) : Hint((Pile)s, (Pile)d, n) { } ///< Initialises hint with integer values.
    };

    /// \brief Represents one physical card.
    ///
    /// Carries information about type of card but also which card is put on this one and where is this card put.
    /// Card structure also provides many methods to manipulate with cards.
    class Card
    {
     private:
        CardInfo info; ///< Represents basic information about card.
        Card *onTop{nullptr}; ///< Represents which card is put on this one.
        Card **base{nullptr}; ///< Represents where is this card put.

     public:
        /// \brief Constructs card if we know rank and color.
        /// \param n Rank of the card to be constructed.
        /// \param c Color of the card to be constructed.
        Card(CardNumber n, CardColor c);

        /// \brief Constructs card if we have one byte representing it.
        ///        Can be used while loading game from file.
        /// \param data Byte representing info about the card to be constructed.
        Card(unsigned char data);

        /// \brief Tells whether the card is top most of the pile.
        /// \return True if no more cards are on this one or false in other cases.
        bool isTopMost();

        /// \brief Get the basic information about this card.
        /// \return The information.
        CardInfo getCardInfo();

        /// \brief Hide the card. It means turn its back side up and front side down.
        /// \return 0 if card was succesfully hiddnen.
        /// \return 1 if card was already hiddnen before.
        /// \return -1 if card cannot be hiddnen because there is another card on it.
        int hide();

        /// \brief Unide the card. It means turn its back side down and front side up.
        /// \return 0 if card was succesfully unhiddnen.
        ///         1 if card was already unhiddnen before.
        ///         -1 if card cannot be unhiddnen because there is another card on it.
        int unhide();

        /// \brief Move this card to the new destination.
        /// \param dest Destination where the card is to be moved on.
        /// \return Move to be saved into history (for case of undo).
        Move putCard(Card **dest);

        /// \brief Recursively get the top most card of the pile.
        /// \return The top most card of the pile.
        Card *getTopMost();

        /// \brief Get the card which is directly on this one.
        /// \return The first next card on this one or nullptr if there is no such card.
        Card *getNext();

        /// \brief Recursively get order of this card counted from top of the pile (used in getNthFromTop).
        /// \retrun The order from the top.
        int getOrder();

        /// \brief Recursively get the Nth card from top of the pile.
        /// \return The Nth card or nullptr if Nth card cannot be found.
        Card *getNthFromTop(int n);

        /// \brief Fill vector of CardInfo with information about cards in this pile.
        /// \param v Reference to the vector to be filled (must be initialised clear before).
        void fillInfoVector(std::vector<CardInfo> &v);

        /// \brief Does the same thing as fillInfoVector but returns string (more like vector of bytes), where each byte represents a card.
        /// \return String representing information about cards in this pile.
        std::string pileToString();
    };

    /// \brief Represents hole one game.
    ///
    /// Carries state of the game which card is put where.
    /// Also provides methods giving information about the state of game and methods executing player's  turns and moves.
    class Game
    {
     private:
        std::vector<Card> pack; ///< Pack of cards. All cards are stored here.
        std::stack<Move> history; ///< History of moves. Store for case of undo.
        Card* stock{nullptr}; ///< Stock is pile of hidden cards prepared to be turned onto waste.
        Card* waste{nullptr}; ///< Cards from stock came here and from her they can be moved to other places of playground.
        Card* foundation[4]{nullptr}; ///< Consists of 4 piles. Goal of the game is to place all cards here.
        Card* tableau[7]{nullptr}; ///< Main part of playground. Cards are organised to 7 piles.
        int score{0}; ///< Score counter.

        /// \brief Moves any card to foundation. Also does most of necessary checks.
        /// \param c Card to be moved.
        /// \param dstPileNumber Number of pile of foundation where card should be moved.
        /// \return 0 in case of success or other number representing type of error.
        int moveToFoundation(Card *c, int dstPileNumber);

        /// \brief Moves one or more cards to tableau. Also does most of necessary checks.
        /// \param c Bottom most card od the pile to be moved.
        /// \param dstPileNumber Number of pile of tableau where card(s) should be moved.
        /// \return 0 in case of success or other number representing type of error.
        int moveToTableau(Card *c, int dstPileNumber);

        /// \brief Writes serie of bytes to the output (usually a file).
        ///        Bytes represent cards from the pile. Pile is ended by a zero byte.
        /// \param output Output stream to be written to.
        /// \param pile Bottom most card of the pile to be saved.
        void writePile(std::ofstream &output, Card *pile);

        /// \brief Reads cards from pack (pack must be loaded from file before) and place them to specified pile.
        /// \param i Pack index to start right after (++i called on the beginning).
        /// \param size Size of pack to prevent index from being out of range.
        /// \param pile Pointer to the pile. Cards shall be put there.
        void loadPile(int &i, int size, Card **pile);

     public:
        /// \brief Constructs new game and shuffles randomly pack of cards and deals the new game.
        Game();

        /// \brief Loads saved game from file and deals the loaded game.
        /// \param filename Path (absolute or relative) to file with saved game.
        Game(std::string filename);

        /// \brief Turns new card from stock to waste.
        /// \return New turned card.
        CardInfo turnNewCard();

        /// \brief Move new card from waste to tableau.
        /// \param dstPileNumber Number of pile of tableau where new card shall be placed.
        /// \return 0 in case of success or other number representing type of error.
        int moveFromWasteToTableau(int dstPileNumber);

        /// \brief Move card back from foundation to tableau.
        /// \param srcPileNumber Number of pile of foundation where card shall be moved from.
        /// \param dstPileNumber Number of pile of tableau where card shall be placed.
        /// \return 0 in case of success or other number representing type of error.
        int moveFromFoundationToTableau(int srcPileNumber, int dstPileNumber);

        /// \brief Move one od more cards from one to another pile of tableau.
        /// \param srcPileNumber Number of pile of tableau where card(s) shall be moved from.
        /// \param dstPileNumber Number of pile of tableau where card(s) shall be placed.
        /// \param numberOfCards Number of cards to be moved.
        /// \return 0 in case of success or other number representing type of error.
        int moveFromTableauToTableau(int srcPileNumber, int dstPileNumber, int numberOfCards);

        /// \brief Submit new card from waste to tableau.
        /// \param dstPileNumber Number of pile of foundation where new card shall be placed.
        /// \return 0 in case of success or other number representing type of error.
        int moveFromWasteToFoundation(int dstPileNumber);

        /// \brief Submit card from foundation to tableau.
        /// \param srcPileNumber Number of pile of tableau where card shall be moved from.
        /// \param dstPileNumber Number of pile of foundation where card shall be placed.
        /// \return 0 in case of success or other number representing type of error.
        int moveFromTableauToFoundation(int srcPileNumber, int dstPileNumber);

        /// \brief Gets value of score which is private variable.
        /// \return Score.
        int getScore();

        /// \brief Tells whether stock is empty or contains some cards.
        /// \return Return true if there are no more cards or false in other cases.
        bool isStockEmpty();

        /// \brief Gets information about cards in waste.
        /// \return Vector of information about cards in waste.
        std::vector<CardInfo> getWaste();

        /// \brief Gets information about cards in one specified pile of foundation.
        /// \param Index of pile of foundation.
        /// \return Vector of information about cards in foundation.
        std::vector<CardInfo> getFoundationPile(int pileIndex);

        /// \brief Gets information about cards in one specified pile of tableau.
        /// \param Index of pile of tableau.
        /// \return Vector of information about cards in tableau.
        std::vector<CardInfo> getTableauPile(int pileIndex);

        /// \brief Finds and gets hint of recommended move or determine if there even are some possible moves.
        /// \return Hint of one recommended move or zero hint in case of the lost game.
        Hint getHint();

        /// \brief Undoes last move which is saved in history.
        /// \return True if undone is successfull or false in other cases.
        bool undo();

        /// \brief Save the game to specified file.
        /// \param filename Path (absolute or relative) to file with saved game.
        void saveGame(std::string filename);
    };
}

#endif