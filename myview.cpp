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
        button[i]->setPos(10+70*i, (height()- 70));
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

    /* Hra2017::Game initialization */
    gameLogic = new Hra2017::Game();
    loadGame();
    //startNewGame();
}

void MyView::loadGame()
{
    for (int i = 0; i < 4; i++) {
        foundation[i] = new MyCard(this);
        foundation[i]->setPixmap(QPixmap(":/img/Resources/placementBorder.png"));
        scene->addItem(foundation[i]);
        std::vector<Hra2017::CardInfo> info = gameLogic->getFoundationPile(i);
        MyCard *temp = foundation[i];
        for (unsigned int x = 0; x < info.size(); x++) {
            MyCard *c = new MyCard(this);
            c->setValue(info[x].hidden, info[x].color, info[x].number);
            scene->addItem(c);
            temp->next = c;
            temp = c;
        }
    }

    for (int z = 0; z < 7; z++) {
        pile[z] = new MyCard(this);
        pile[z]->setPixmap(QPixmap(":/img/Resources/placementBorder.png"));
        scene->addItem(pile[z]);
        std::vector<Hra2017::CardInfo> info = gameLogic->getTableauPile(z);
        MyCard *temp = pile[z];
        for (unsigned int x = 0; x < info.size(); x++) {
            MyCard *c = new MyCard(this);
            c->setValue(info[x].hidden, info[x].color, info[x].number);
            scene->addItem(c);
            temp->next = c;
            c->prev = temp;
            temp = c;
        }
    }

    std::vector<Hra2017::CardInfo> info = gameLogic->getWaste();
    for (unsigned int i = 0; i < info.size(); i++) {

    }

    layoutCards(1);

}

void MyView::startNewGame()
{
    if (gameState == 1) {
        return;
    }

    for (int z = 0; z < 7; z++)
    {
        pile[z] = new MyCard(this);
        MyCard *c = pile[z];
        MyCard *temp = NULL;
        for (int i = 0; i <= z; i++)
        {
            c->prev = temp;
            c->next = new MyCard(this);
            c->setZValue(i);
            scene->addItem(c);
            cards.append(c);
            temp = c;
            c = c->next;
        }
        c->setZValue(z);
        scene->addItem(c);
        cards.append(c);
        c->setValue(1, 4, 4);
    }

    for (int i = 0; i < 4; i++) {
        foundation[i] = new MyCard();
        foundation[i]->setPixmap(QPixmap(":/img/Resources/placementBorder.png"));
        scene->addItem(foundation[i]);
    }

    layoutCards(1);

    gameState = 1;
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
    qreal cardHeight = 243*pile[0]->scale()*scalingRatio + 15;
    for (int z = 0; z < 7; z++)
    {
        int i = 0;
        pile[z]->setPos(width()*0.02 + z*stepW, cardHeight);
        pile[z]->setScale(pile[z]->scale()*scalingRatio);
        for (MyCard *c = pile[z]->next; c; i++, c = c->next) {
            c->setPos(width()*0.02 + z*stepW, cardHeight + 20*i);
            c->setScale(c->scale()*scalingRatio);
        }
    }

    for (int z = 0; z < 4; z++)
    {
        int i = 0;
        for (MyCard *c = foundation[z]; c; i++, c = c->next) {
            c->setPos(width()*0.02 +3*stepW + z*stepW, 10);
            c->setScale(c->scale()*scalingRatio);
        }
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

void MyView::removeButtonPressed()
{
    resetGame();
    gameState = 0;
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
    qDebug() << a;
}

void MyView::saveButtonPressed()
{
    QFileDialog *dialog = new QFileDialog(this);
    dialog->show();
    connect(dialog, SIGNAL(fileSelected(QString)), this, SLOT(saveSelected(QString)));

}

void MyView::saveSelected(QString a)
{
    qDebug() << a;
}

void MyView::resetGame()
{
    for (int z = 0; z < 7; z++) {
        MyCard *temp = pile[z];
        for (MyCard *c = temp->next; c; c = c->next) {
            delete temp;
            temp = c;
        }
        delete temp;
    }

    for (int i = 0; i < 6; i++) {
        delete button[i];
    }

    for (int z = 0; z < 4; z++) {
        MyCard *temp = foundation[z];
        for (MyCard *c = temp->next; c; c = c->next) {
            delete temp;
            temp = c;
        }
        delete temp;
    }

}
