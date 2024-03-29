/// \file myview.cpp
/// \author xverny00
/// \brief Implementation of custom variant of QGraphicsView

#include "myview.h"

#include <QDebug>

MyView::MyView()
{
    /* set up scene */
    scene = new QGraphicsScene();

    this->setScene(scene);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setMinimumSize(640, 480);
    scene->setSceneRect(0,0, 640, 480);
    scene->setBackgroundBrush(QPixmap(":/img/Resources/background.png").scaled(640,480, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    /* clear memory */
    memset(pile, 0, sizeof(MyCard*)*7);
    memset(foundation, 0, sizeof(MyCard*)*4);
    memset(button, 0, sizeof(MyButton*)*6);

    /* init buttons */
    for (int i = 0; i < 5; i++) {
        button[i] = new MyButton(this);
        scene->addItem(button[i]);
    }

    button[5] = new MyButton(this);
    button[5]->setURL(":/img/Resources/addGamebutton.png");
    button[5]->setPos(560, (height()- 70));
    scene->addItem(button[5]);

    button[0]->setURL(":/img/Resources/ngButton.png");
    button[1]->setURL(":/img/Resources/loadButton.png");
    button[2]->setURL(":/img/Resources/saveButton.png");
    button[3]->setURL(":/img/Resources/undoButton.png");
    button[4]->setURL(":/img/Resources/hintButton.png");

    connect(button[0], SIGNAL(buttonPressed()), this, SLOT(removeButtonPressed()));
    connect(button[1], SIGNAL(buttonPressed()), this, SLOT(loadButtonPressed()));
    connect(button[2], SIGNAL(buttonPressed()), this, SLOT(saveButtonPressed()));
    connect(button[3], SIGNAL(buttonPressed()), this, SLOT(undoButtonPressed()));
    connect(button[4], SIGNAL(buttonPressed()), this, SLOT(hintButtonPressed()));

    stock = new MyButton(this);
    stock->setURL(":/img/Resources/Cover.png");
    stock->setScale(0.35);
    scene->addItem(stock);
    connect(stock, SIGNAL(buttonPressed()), this, SLOT(stockToFoundation()));


    /* Hra2017::Game initialization */
    gameLogic = new Hra2017::Game();
    loadGame();
}

void MyView::loadGame()
{
    /* load foundation */
    for (int i = 0; i < 4; i++) {
        foundation[i] = new MyCard(8 + i, this);
        foundation[i]->setPixmap(QPixmap(":/img/Resources/placementBorder.png"));
        scene->addItem(foundation[i]);
        std::vector<Hra2017::CardInfo> info = gameLogic->getFoundationPile(i);
        MyCard *temp = foundation[i];
        for (unsigned int x = 0; x < info.size(); x++) {
            MyCard *c = new MyCard(8 + i, this);
            c->setValue(info[x].hidden, info[x].color, info[x].number);
            scene->addItem(c);
            temp->next = c;
            c->prev = temp;
            temp = c;
        }
    }

    /* load piles */
    for (int z = 0; z < 7; z++) {
        pile[z] = new MyCard(1 + z, this);
        pile[z]->setPixmap(QPixmap(":/img/Resources/placementBorder.png"));
        scene->addItem(pile[z]);
        std::vector<Hra2017::CardInfo> info = gameLogic->getTableauPile(z);
        MyCard *temp = pile[z];
        for (unsigned int x = 0; x < info.size(); x++) {
            MyCard *c = new MyCard(1+z, this);
            c->setValue(info[x].hidden, info[x].color, info[x].number);
            scene->addItem(c);
            temp->next = c;
            c->prev = temp;
            temp = c;
        }
    }

    /* load waste */
    waste = new MyCard(0, this);
    waste->setPixmap(QPixmap(":/img/Resources/placementBorder.png"));
    scene->addItem(waste);
    std::vector<Hra2017::CardInfo> vector = gameLogic->getWaste();
    MyCard *temp = waste;
    for (unsigned int x = 0; x < vector.size(); x++) {
        MyCard *c = new MyCard(0, this);
        c->setValue(vector[x].hidden, vector[x].color, vector[x].number);
        scene->addItem(c);
        temp ->next = c;
        c->prev = temp;
        temp = c;
    }

    layoutCards(1);

}

void MyView::resizeEvent(QResizeEvent *event)
{
    if (-1 != event->oldSize().height())
    {
        qreal a = event->size().height();
        a /= event->oldSize().height();
        layoutCards(a);
    }

    if (-1 != event->oldSize().width())
    {
        layoutCards(1);
    }
}

void MyView::layoutCards(qreal scalingRatio)
{
    qreal stepW = (width() - width()*0.04) / 7;
    qreal cardHeight = 243*pile[0]->scale()*scalingRatio + height()*0.02 + 10;
    for (int z = 0; z < 7; z++)
    {
        int i = 0;
        pile[z]->setPos(width()*0.02 + z*stepW, cardHeight);
        pile[z]->setScale(pile[z]->scale()*scalingRatio);
        for (MyCard *c = pile[z]->next; c; i++, c = c->next) {
            c->setPos(width()*0.02 + z*stepW, cardHeight + 20*i);
            c->setScale(c->scale()*scalingRatio);
            c->setZValue(i+1);
        }
    }

    for (int z = 0; z < 4; z++)
    {
        int i = 0;
        for (MyCard *c = foundation[z]; c; i++, c = c->next) {
            c->setPos(width()*0.02 +3*stepW + z*stepW, height()*0.02);
            c->setScale(c->scale()*scalingRatio);
            c->setZValue(i+1);
        }
    }

    stock->setPos(width()*0.02, height()*0.02);
    stock->setScale(stock->scale()*scalingRatio);

    int i = 1;
    for (MyCard *c = waste; c; c = c->next) {
        c->setPos(width()*0.02 +1*stepW , height()*0.02);
        c->setScale(c->scale()*scalingRatio);
        c->setZValue(i);
        i++;
    }

    qreal buttonSize = 300*button[0]->scale();
    for (int z = 0; z < 5; z++) {
        button[z]->setPos(width()*0.02 + z*buttonSize + z*10, height() - buttonSize - height()*0.02);
        button[z]->setScale(button[z]->scale()*scalingRatio);
    }
    button[5]->setPos(width() - width()*0.02 - buttonSize, height() - buttonSize - height()*0.02);
    button[5]->setScale(button[5]->scale()*scalingRatio);
}

MyButton *MyView::getButton5()
{
    return button[5];
}

int MyView::getGameState()
{
    return gameState;
}

Hra2017::Game *MyView::getGameLogic()
{
    return gameLogic;
}

void MyView::removeButtonPressed()
{
    resetGame();
    gameState = 0;
    delete gameLogic;
    emit stopButtonPressed();
}

void MyView::loadButtonPressed()
{
    QFileDialog *dialog = new QFileDialog(this);
    dialog->show();
    connect(dialog, SIGNAL(fileSelected(QString)), this, SLOT(loadSelected(QString)));
}

void MyView::loadSelected(QString a)
{
    Hra2017::Game *temp = NULL;
    try {
        temp = new Hra2017::Game(a.toStdString());
    }
    catch (const char *e) {
        return;
    }
    softResetGame();
    delete gameLogic;
    gameLogic = temp;
    loadGame();
}

void MyView::saveButtonPressed()
{
    QFileDialog *dialog = new QFileDialog(this);
    dialog->show();
    connect(dialog, SIGNAL(fileSelected(QString)), this, SLOT(saveSelected(QString)));
}

void MyView::saveSelected(QString a)
{
    gameLogic->saveGame(a.toStdString());
}

void MyView::undoButtonPressed()
{
    if (gameLogic->undo()) {
        qDebug() << "UNDO";
        softResetGame();
        loadGame();
    }
}

void MyView::hintButtonPressed()
{
    static int i = 0;
    if (i == 0) {
        Hra2017::Hint h = gameLogic->getHint();
        QString *str = new QString("HINT:\n");
        if (h.destination == 0) {
            str->append("Turn ");
        } else {
            str->append("Move ");
        }
        str->append(QString::number(h.number));
        if (h.number == 1) {
            str->append(" card from ");
        } else {
            str->append(" cards from ");
        }
        if (h.source == 0) {
            str->append("waste");
        }
        if (h.source > 0 and h.source < 8) {
            str->append("pile ");
            str->append(QString::number(h.source));
        }
        if (h.source > 7 and h.source < 12) {
            str->append("foundation ");
            str->append(QString::number(h.source-7));
        }
        if (h.destination != 0)
            str->append(" to ");

        if (h.destination > 0 and h.destination < 8) {
            str->append("pile ");
            str->append(QString::number(h.destination));
        }
        if (h.destination > 7 and h.destination < 12) {
            str->append("foundation");
        }
        str->append(".");

        QMessageBox *m = new QMessageBox();
        m->setText(*str);
        m->exec();
        delete str;
        delete m;
        i++;
    } else {
        i--;
    }
}

void MyView::stockToFoundation()
{
    if (!gameLogic->isStockEmpty()) {
        Hra2017::CardInfo info = gameLogic->turnNewCard();
        MyCard *temp;
        for ( temp = waste; temp->next != NULL; temp = temp->next) {}
        temp->next = new MyCard(0, this);
        temp->next->setScale(temp->scale());
        temp->next->prev = temp;
        temp = temp->next;
        temp->setValue(info.hidden, info.color, info.number);
        scene->addItem(temp);
        if (gameLogic->isStockEmpty()) {
            stock->setPixmap(QPixmap(":/img/Resources/Joker.png"));
            Hra2017::Hint h = gameLogic->getHint();
            if (h.number ==  0) {
                //YOU LOSE
                QMessageBox *message = new QMessageBox();
                QString *str = new QString("YOU LOSE!\nYour score: ");
                int score = gameLogic->getScore();
                str->append(QString::number(score));
                message->setText(*str);
                message->show();
            }
        }
    } else {
        Hra2017::Hint h = gameLogic->getHint();
        if (h.number ==  0) {
            //YOU LOSE
            QMessageBox *message = new QMessageBox();
            QString *str = new QString("YOU LOSE!\nYour score: ");
            int score = gameLogic->getScore();
            str->append(QString::number(score));
            message->setText(*str);
            message->show();
        }
        /* free waste */
        stock->setPixmap(QPixmap(":/img/Resources/Cover.png"));
        if (waste->next != NULL and waste->next->next != NULL) {
            MyCard *temp = waste->next;
            for (MyCard *c = temp->next; c; c = c->next) {
                delete temp;
                temp = c;
            }
            delete temp;
            waste->next = NULL;
        }
        gameLogic->turnNewCard();
    }
    layoutCards(1);
}

void MyView::softResetGame()
{
    /* free piles */
    for (int z = 0; z < 7; z++) {
        MyCard *temp = pile[z];
        for (MyCard *c = temp->next; c; c = c->next) {
            delete temp;
            temp = c;
        }
        delete temp;
    }

    /* free foundation */
    for (int z = 0; z < 4; z++) {
        MyCard *temp = foundation[z];
        for (MyCard *c = temp->next; c; c = c->next) {
            delete temp;
            temp = c;
        }
        delete temp;
    }

    /* free waste */
    MyCard *temp = waste;
    for (MyCard *c = temp->next; c; c = c->next) {
        delete temp;
        temp = c;
    }
    delete temp;
}

void MyView::resetGame()
{
    /* free piles */
    for (int z = 0; z < 7; z++) {
        MyCard *temp = pile[z];
        for (MyCard *c = temp->next; c; c = c->next) {
            delete temp;
            temp = c;
        }
        delete temp;
    }

    /* free buttons */
    for (int i = 0; i < 6; i++) {
        delete button[i];
    }

    /* free foundation */
    for (int z = 0; z < 4; z++) {
        MyCard *temp = foundation[z];
        for (MyCard *c = temp->next; c; c = c->next) {
            delete temp;
            temp = c;
        }
        delete temp;
    }

    /* free waste */
    MyCard *temp = waste;
    for (MyCard *c = temp->next; c; c = c->next) {
        delete temp;
        temp = c;
    }
    delete temp;

    /* free stock button */
    delete stock;
}
