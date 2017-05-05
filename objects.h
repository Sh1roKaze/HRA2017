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

    enum CardColor
    {
        hearts = 0,
        diamonds = 1, tiles = 1,
        clubs = 2, clovers = 2,
        spades = 3, pikes = 3
    };

    struct CardInfo
    {
        CardNumber number{none};
        CardColor color{hearts};
        bool hidden{false};
    };
    
    class Card;
    struct Move
    {
        Card *card;
        Card **from;
        int score;
    };

    class Card
    {
     private:
        CardInfo info;
        Card *onTop{nullptr};
        Card **base{nullptr};

     public:
        Card(CardNumber n, CardColor c);

        Card(char data);

        bool isTopMost();

        CardInfo getCardInfo();

        int hide();

        int unhide();

        Move putCard(Card **dest);

        Card *getTopMost();

        Card *getNthFromTop(int n);

        int getOrder();

        void fillInfoVector(std::vector<CardInfo> &v);

        char toChar();
    };

    class Game
    {
     private:
        std::vector<Card> pack;
        std::stack<Move> history;
        Card* stock{nullptr};
        Card* waste{nullptr};
        Card* foundation[4]{nullptr};
        Card* tableau[7];
        int score{0};

        int moveToFoundation(Card *c, int dstPileNumber);
        int moveToTableau(Card *c, int dstPileNumber);

     public:
        Game();
        Game(std::string filename);

        void turnNewCard();
        int moveFromWasteToTableau(int dstPileNumber);
        int moveFromFoundationToTableau(int srcPileNumber, int dstPileNumber);
        int moveFromTableauToTableau(int srcPileNumber, int dstPileNumber, int numberOfCards);
        int moveFromWasteToFoundation(int dstPileNumber);
        int moveFromTableauToFoundation(int srcPileNumber, int dstPileNumber);

        int getScore();
        bool isStockEmpty();
        CardInfo getWasteTop();
        std::vector<CardInfo> getFoundation();
        std::vector<CardInfo> getTableauPile(int pileIndex);

        bool undo();

        void saveGame(std::string filename);
    };
}

#endif