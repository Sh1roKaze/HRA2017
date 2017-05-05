// ICP Project
// Objects representing state of game
// xvlach16

#include "objects.h"

namespace Hra2017
{
    Card::Card(CardNumber n, CardColor c)
    {
        info.number = n;
        info.color = c;
        info.hidden = false;
    }

    Card::Card(char data)
    {
        int num = data & 15;
        int col = (data & 48) >> 4;
        bool hid = ((data & 64) >> 6) == 1;

        if (num < 1 || num > 13)
            throw "BAD_NUMBER";
        if (col < 0 || col > 3)
            throw "BAD_COLOR";

        info.number = (CardNumber)num;
        info.color = (CardColor)col;
        info.hidden = hid;
    }

    bool Card::isTopMost()
    {
        // no more cards on top
        return onTop == nullptr;
    }

    CardInfo Card::getCardInfo()
    {
        // info is read only
        return info;
    }

    int Card::hide()
    {
        if (info.hidden)
            return 1; // ALREDY_DONE
        if (onTop != nullptr)
            return 2; // NOT_TOP_MOST

        info.hidden = true;
        return 0; // SUCCESS
    }

    int Card::unhide()
    {
        if (!info.hidden)
            return 1; // ALREDY_DONE
        if (onTop != nullptr)
            return 2; // NOT_TOP_MOST

        info.hidden = false;
        return 0; // SUCCESS
    }

    Move Card::putCard(Card **dest)
    {
        if (*dest != nullptr)
            return putCard(&(*dest)->onTop);
        else
        {
            Move move{this, base, 0};

            *dest = this;
            if (this->base != nullptr)
                *this->base = nullptr;
            this->base = dest;

            return move;
        }
    }

    Card *Card::getTopMost()
    {
        if (onTop == nullptr)
            return this;
        
        return onTop->getTopMost();
    }

    Card *Card::getNthFromTop(int n)
    {
        int order = getOrder();
        if (order < n)
            return nullptr;
        if (order > n)
            return onTop->getNthFromTop(n);

        return this;
    }

    int Card::getOrder()
    {
        if (onTop == nullptr)
            return 1;

        return onTop->getOrder() + 1;
    }

    void Card::fillInfoVector(std::vector<CardInfo> &v)
    {
        v.push_back(info);

        if (onTop != nullptr)
            onTop->fillInfoVector(v);
    }

    char Card::toChar()
    {
        // Turns all data in one byte
        return (char)(this->info.number | (this->info.color << 4) | ((this->info.hidden ? 1 : 0) << 6));
    }

    //=================================================================================================

    int Game::moveToFoundation(Card *c, int dstPileNumber)
    {
        if (foundation[dstPileNumber] == nullptr)
        {
            if (c->getCardInfo().number != CardNumber::ace)
                return 4; // MUST_BE_ACE
        }
        else
        {
            CardInfo pileTopInfo = foundation[dstPileNumber]->getTopMost()->getCardInfo();
            if (c->getCardInfo().number != pileTopInfo.number + 1)
                return 2; // BAD_NUMBER
            if (c->getCardInfo().color != pileTopInfo.color)
                return 3; // BAD_COLOR
        }

        Move move = c->putCard(&foundation[dstPileNumber]);
        move.score = score;
        history.push(move);
        score += 10;

        return 0; //SUCCESS
    }

    int Game::moveToTableau(Card *c, int dstPileNumber)
    {
        if (tableau[dstPileNumber] == nullptr)
        {
            if (c->getCardInfo().number != CardNumber::king)
                return 4; // MUST_BE_KING
        }
        else
        {
            CardInfo pileTopInfo = tableau[dstPileNumber]->getTopMost()->getCardInfo();
            if (c->getCardInfo().number != pileTopInfo.number - 1)
                return 2; // BAD_NUMBER
            if ((c->getCardInfo().color & 2) != (pileTopInfo.color & 2))
                return 3; // BAD_COLOR
        }
        
        Move move = c->putCard(&tableau[dstPileNumber]);
        move.score = score;
        history.push(move);

        return 0; //SUCCESS
    }

    Game::Game()
    {
        for (int i = 0; i < 52; i++)
            pack.push_back(Card((CardNumber)(i % 13 + 1), (CardColor)(i / 13)));

        std::random_shuffle(pack.begin(), pack.end());

        unsigned i2 = 0;
        for (int p = 0; p < 7; p++)
        {
            for (int i = 0; i <= p; i++)
                pack[i2++].putCard(&tableau[p]);
            tableau[p]->getTopMost()->unhide();
        }

        while (i2 < pack.size())
            pack[i2++].putCard(&stock);
    }

    /*Game::Game(std::string filename)
    {

    }*/

    void Game::turnNewCard()
    {
        history.push({nullptr, nullptr, score});

        if (stock == nullptr)
        {
            while(stock != nullptr)
            {
                Card *c = waste->getTopMost();
                c->putCard(&stock);
                c->hide();
            }

            score -= score < 100 ? score : 100;
        }
        else
        {
            Card *c = stock->getTopMost();
            c->putCard(&waste);
            c->unhide();
        }
    }

    int Game::moveFromWasteToTableau(int dstPileNumber)
    {
        if (waste == nullptr)
            return 1; // SRC_EMPTY

        Card *c = waste->getTopMost();

        int ret = moveToTableau(c, dstPileNumber);

        if (ret == 0)
            score += 5;

        return ret;
    }

    int Game::moveFromFoundationToTableau(int srcPileNumber, int dstPileNumber)
    {
        if (foundation[srcPileNumber] == nullptr)
            return 1; // SRC_EMPTY

        Card *c = foundation[srcPileNumber]->getTopMost();

        int ret = moveToTableau(c, dstPileNumber);

        if (ret == 0)
            score -= score < 15 ? score : 15;

        return ret;
    }

    int Game::moveFromTableauToTableau(int srcPileNumber, int dstPileNumber, int numberOfCards)
    {
        if (tableau[srcPileNumber] == nullptr)
            return 1; // SRC_EMPTY

        Card *c = tableau[srcPileNumber]->getNthFromTop(numberOfCards);
        if (c == nullptr)
            return 5; // NOT_ENOUGH_CARDS

        int ret = moveToTableau(c, dstPileNumber);

        if (ret == 0 && tableau[srcPileNumber] != nullptr)
        {
            c = tableau[srcPileNumber]->getTopMost();
            if (c->getCardInfo().hidden)
            {
                c->unhide();
                score += 5;
            }
        }

        return ret;
    }

    int Game::moveFromWasteToFoundation(int dstPileNumber)
    {
        if (waste == nullptr)
            return 1; // SRC_EMPTY

        Card *c = waste->getTopMost();

        return moveToFoundation(c, dstPileNumber);
    }

    int Game::moveFromTableauToFoundation(int srcPileNumber, int dstPileNumber)
    {
        if (tableau[srcPileNumber] == nullptr)
            return 1; // SRC_EMPTY

        Card *c = tableau[srcPileNumber]->getTopMost();

        return moveToFoundation(c, dstPileNumber);
    }

    int Game::getScore()
    {
        // score is read only
        return score;
    }

    bool Game::isStockEmpty()
    {
        // user doesn't need to know, all cards ins stock...
        return stock == nullptr;
    }

    CardInfo Game::getWasteTop()
    {
        CardInfo info{};
        if (waste != nullptr)
            info = waste->getTopMost()->getCardInfo();
        return info;
    }

    std::vector<CardInfo> Game::getFoundation()
    {
        std::vector<CardInfo> infos;

        for (int i = 0; i < 4; i++)
            if (foundation[i] != nullptr)
                infos.push_back(foundation[i]->getTopMost()->getCardInfo());
            else
                infos.push_back(CardInfo());

        return infos;
    }

    std::vector<CardInfo> Game::getTableauPile(int pileIndex)
    {
        std::vector<CardInfo> infos;

        if (tableau[pileIndex] != nullptr)
            tableau[pileIndex]->fillInfoVector(infos);

        return infos;
    }

    bool Game::undo()
    {
        if (history.empty())
            return false;

        Move move = history.top();
        history.pop();

        if (move.card == nullptr)
        {
            if (waste == nullptr)
            {
                while(waste != nullptr)
                {
                    Card *c = stock->getTopMost();
                    c->putCard(&waste);
                    c->unhide();
                }
            }
            else
            {
                Card *c = waste->getTopMost();
                c->putCard(&stock);
                c->hide();
            }
        }
        else
            move.card->putCard(move.from);

        score = move.score;
        return true;
    }

    /*void saveGame(std::string filename)
    {

    }*/
}