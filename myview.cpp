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
    startNewGame();
}

void MyView::loadGame()
{
/*    for (int i = 0; i < 4; i++) {
        std::vector<Hra2017::CardInfo> info = gameLogic->getFoundation(i);
        MyCard *temp = foundation[i];
        for (unsigned int x = 0; x < info.size(); x++) {
            MyCard *c = new MyCard();
            c->setValue(info[x].color, info[x].number);
            temp->next = c;
            temp = c;
        }
    }

    for (int z = 0; z < 7; z++) {
        std::vector<Hra2017::CardInfo> info = gameLogic->getTableauPile(z);
        MyCard *temp = pile[z];
        for (unsigned int x = 0; x < info.size(); x++) {
            MyCard *c = new MyCard();
            c->setValue(info[x].color, info[x].number);
            temp->next = c;
            temp = c;
        }
    }

    std::vector<Hra2017::CardInfo> info = gameLogic->getWaste();
    for (int i = 0; i < info.size(); i++) {

    }
*/
    layoutCards();

}

void MyView::startNewGame()
{
    if (gameState == 1) {
        return;
    }

    float step = width() / 8;
    for (int z = 0; z < 7; z++)
    {
        pile[z] = new MyCard(this);
        pile[z]->setPos(step/2 + z*step, 100);
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
        c->setValue(4, 4);
    }

    for (int i = 0; i < 4; i++) {
        foundation[i] = new MyCard();
        foundation[i]->setPixmap(QPixmap(":/img/Resources/placementBorder.png"));
        scene->addItem(foundation[i]);
    }

    layoutCards();

    gameState = 1;
}

void MyView::resizeEvent(QResizeEvent *event)
{
    if (-1 != event->oldSize().height())
    {
        float a = event->size().height();
        a = a/event->oldSize().height();
        for (int i = 0; i < cards.count(); ++i) {
            cards[i]->setScale(cards[i]->scale()*a);
        }
        for (int i = 0; i < 6; i++) {
            button[i]->setScale(button[i]->scale()*a);
        }
    }

    if (-1 != event->oldSize().width())
    {
        layoutCards();
    }
}

void MyView::layoutCards()
{
    float stepW = this->width() / 8;
    for (int z = 0; z < 7; z++)
    {
        if (pile[z] == NULL) { continue; }
        pile[z]->setPos(stepW/2 + z*stepW, 100);
        int i = 0;
        for (MyCard *c = pile[z]; c; i++, c = c->next)
            c->setPos(stepW/2 + z*stepW, 100 + 20*i);
    }

    for (int z = 0; z < 4; z++)
    {
        if (foundation[z] == NULL) { continue; }
        foundation[z]->setPos(stepW/2 + z*stepW, 100);
        int i = 0;
        for (MyCard *c = foundation[z]; c; i++, c = c->next)
            c->setPos(stepW/2 +3*stepW + z*stepW, 10);
    }

    qreal a = 300*button[0]->scale();
    for (int z = 0; z < 5; z++) {
        button[z]->setPos(stepW/2 + z*a + z*10, height() - a - 10);
    }
    button[5]->setPos(width() - stepW/2 - a, height() - a - 10);
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

    for (int i = 2; i < 5; i++) {
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
