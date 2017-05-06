// ICP Project
// Objects representing state of game
// xvlach16

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "objects.h"

namespace Hra2017
{
    Card::Card(CardNumber n, CardColor c)
    {
        info.number = n;
        info.color = c;
        info.hidden = true;
    }

    Card::Card(unsigned char data)
    {
        int num = data & 15;
        int col = (data & 48) >> 4;
        bool hid = ((data & 64) >> 6) == 1;

        if (num < 0 || num > 13 || col < 0 || col > 3 || data > 127)
            throw "BAD_DATA";

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

    std::string Card::pileToString()
    {
        std::string s;
        s += (char)(this->info.number | (this->info.color << 4) | ((this->info.hidden ? 1 : 0) << 6));
        
        if (onTop != nullptr)
            s += onTop->pileToString();

        return s;
    }

    //=================================================================================================

    int Game::moveToFoundation(Card *c, int dstPileNumber)
    {
        if (c == nullptr)
            return 5; // CANNOT_MOVE_NULL
        if (c->getCardInfo().hidden)
            return 6; // CANNOT_MOVE_HIDDEN_CARD

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
        if (c == nullptr)
            return 5; // CANNOT_MOVE_NULL
        if (c->getCardInfo().hidden)
            return 6; // CANNOT_MOVE_HIDDEN_CARD

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
            if ((c->getCardInfo().color & 2) == (pileTopInfo.color & 2))
                return 3; // BAD_COLOR
        }
        
        Move move = c->putCard(&tableau[dstPileNumber]);
        move.score = score;
        history.push(move);

        return 0; //SUCCESS
    }

    void Game::loadPile(int &i, int size, Card **pile)
    {
        while (++i < size)
        {
            if (pack[i].getCardInfo().number != CardNumber::none)
                pack[i].putCard(pile);
            else
                return;
        }
        throw "MISSING_DATA";
    }

    void Game::writePile(std::ofstream &output, Card *pile)
    {
        std::string s = "";

        if (pile != nullptr)
            s = pile->pileToString();

        for (int i = 0; i < (int)s.length(); i++)
            output.put(s[i]);

        output.put('\0');
    }

    Game::Game()
    {
        for (int i = 0; i < 52; i++)
            pack.push_back(Card((CardNumber)(i % 13 + 1), (CardColor)(i / 13)));

        srand(time(NULL));
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

    Game::Game(std::string filename)
    {
        std::ifstream input(filename, std::ios::in | std::ios::binary);

        score = input.get() << 8;
        score |= input.get();
        if (score < 0)
            throw "MISSING_DATA";

        int c;
        while ((c = input.get()) >= 0)
            pack.push_back(Card(c));

        input.close();

        int idx{-1};
        loadPile(idx, pack.size(), &stock);
        loadPile(idx, pack.size(), &waste);

        for(int i = 0; i < 4; i++)
            loadPile(idx, pack.size(), &foundation[i]);

        for (int i = 0; i < 7; i++)
            loadPile(idx, pack.size(), &tableau[i]);
    }

    CardInfo Game::turnNewCard()
    {
        if (stock != nullptr)
        {
            Card *c = stock->getTopMost();
            c->putCard(&waste);
            c->unhide();

            history.push({nullptr, nullptr, score});
            return c->getCardInfo();
        }
        else if (waste != nullptr) 
        {
            while(waste != nullptr)
            {
                Card *c = waste->getTopMost();
                c->putCard(&stock);
                c->hide();
            }

            history.push({nullptr, nullptr, score});
            score -= score < 100 ? score : 100;
            return CardInfo();
        }

        return CardInfo();
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
        if (numberOfCards <= 0)
            return 7; // NUMBER_NOT_POSITIVE
        if (tableau[srcPileNumber] == nullptr)
            return 1; // SRC_EMPTY

        Card *c = tableau[srcPileNumber]->getNthFromTop(numberOfCards);

        int ret = moveToTableau(c, dstPileNumber);

        if (ret == 0 && tableau[srcPileNumber] != nullptr)
        {
            c = tableau[srcPileNumber]->getTopMost();
            if (c->getCardInfo().hidden)
            {
                c->unhide();
                history.push({c, nullptr, 0});
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

        int ret = moveToFoundation(c, dstPileNumber);

        if (ret == 0 && tableau[srcPileNumber] != nullptr)
        {
            c = tableau[srcPileNumber]->getTopMost();
            if (c->getCardInfo().hidden)
            {
                c->unhide();
                history.push({c, nullptr, 0});
                score += 5;
            }
        }

        return ret;
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

    std::vector<CardInfo> Game::getWaste()
    {
        std::vector<CardInfo> infos;

        if (waste != nullptr)
            waste->fillInfoVector(infos);

        return infos;
    }

    std::vector<CardInfo> Game::getFoundationPile(int pileIndex)
    {
        if (pileIndex < 0 || pileIndex > 3)
            throw "INDEX_OUT_OF_RANGE";

        std::vector<CardInfo> infos;

        if (foundation[pileIndex] != nullptr)
            foundation[pileIndex]->fillInfoVector(infos);

        return infos;
    }

    std::vector<CardInfo> Game::getTableauPile(int pileIndex)
    {
        if (pileIndex < 0 || pileIndex > 7)
            throw "INDEX_OUT_OF_RANGE";

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

        if (move.from == nullptr)
        {
            move.card->hide();
            move = history.top();
            history.pop();
        }

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

    void Game::saveGame(std::string filename)
    {
        std::ofstream output(filename, std::ios::out | std::ios::binary);

        output.put((char)(score >> 8));
        output.put((char)(score & 255));

        writePile(output, stock);
        writePile(output, waste);

        for(int i = 0; i < 4; i++)
            writePile(output, foundation[i]);

        for (int i = 0; i < 7; i++)
            writePile(output, tableau[i]);
        
        output.close();
    }
}