#include "myview.h"

#include <QDebug>

MyView::MyView()
{
    /* set up scene */
    scene = new QGraphicsScene();
    scene->setBackgroundBrush(Qt::green);

    this->setScene(scene);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setMinimumSize(640, 480);
    scene->setSceneRect(0,0, 640, 480);

    /* clear memory */
    memset(pile, 0, sizeof(Card*)*7);
    memset(foundation, 0, sizeof(Card*)*4);
    memset(button, 0, sizeof(MyButton*)*6);

    /* init start and stop buttons */
    button[0] = new MyButton(this);
    button[0]->setURL(":/img/Resources/ngButton.png");

    button[5] = new MyButton(this);
    button[5]->setURL(":/img/Resources/addGamebutton.png");
    button[5]->setPos(560, (height()- 70));
    scene->addItem(button[5]);

    button[0]->setPos(10, (height()- 70));
    scene->addItem(button[0]);
    connect(button[0], SIGNAL(buttonPressed()), this, SLOT(removeButtonPressed()));

    startNewGame();
}

void MyView::startNewGame()
{
    if (reset == 1) {
        return;
    }

    for (int i = 1; i < 5; i++) {
        button[i] = new MyButton(this);
        button[i]->setPos(10+70*i, (height()- 70));
        scene->addItem(button[i]);
    }

    button[1]->setURL(":/img/Resources/loadButton.png");
    button[2]->setURL(":/img/Resources/saveButton.png");
    button[3]->setURL(":/img/Resources/undoButton.png");
    button[4]->setURL(":/img/Resources/hintButton.png");

    connect(button[1], SIGNAL(buttonPressed()), this, SLOT(loadButtonPressed()));
    connect(button[2], SIGNAL(buttonPressed()), this, SLOT(saveButtonPressed()));

    float step = width() / 8;
    for (int z = 0; z < 7; z++)
    {
        pile[z] = new Card(this);
        pile[z]->setPos(step/2 + z*step, 100);
        Card *c = pile[z];
        Card *temp = NULL;
        for (int i = 0; i < z; i++)
        {
            c->prev = temp;
            c->next = new Card(this);
            c->setZValue(i);
            scene->addItem(c);
            cards.append(c);
            c->setPos(step/2 + z*step, 100 + 20*i);
            temp = c;
            c = c->next;
        }
        c->setZValue(z);
        c->next = NULL;
        scene->addItem(c);
        cards.append(c);
        c->setPos(step/2 + z*step, 100 + 20*z);
        c->setValue(4);
    }

    for (int i = 0; i < 4; i++) {
        foundation[i] = new Card();
        foundation[i]->setPos(step/2 + 3*step + i*step, 10);
        foundation[i]->setPixmap(QPixmap(":/img/Resources/placementBorder.png"));
        scene->addItem(foundation[i]);
    }

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
        for (int i = 0; i < 5; i++) {
            if (button[i] == NULL) {
                break;
            }
            button[i]->setScale(button[i]->scale()*a);
            button[i]->setPos(button[i]->pos().x(), height() - button[i]->y()*button[i]->scale());
        }
    }

    if (-1 != event->oldSize().width())
    {
        float newStep = this->width() / 8;
        for (int z = 0; z < 7; z++)
        {
            if (pile[z] == NULL) { continue; }
            pile[z]->setPos(newStep/2 + z*newStep, 100);
            int i = 0;
            for (Card *c = pile[z]; c; i++, c = c->next)
                c->setPos(newStep/2 + z*newStep, 100 + 20*i);
        }
    }
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
    reset = 0;
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
        Card *temp = pile[z];
        for (Card *c = temp->next; c; c = c->next) {
            delete temp;
            temp = c;
        }
        delete temp;
    }

    for (int i = 2; i < 5; i++) {
        delete button[i];
    }

    for (int z = 0; z < 4; z++) {
        Card *temp = foundation[z];
        for (Card *c = temp->next; c; c = c->next) {
            delete temp;
            temp = c;
        }
        delete temp;
    }

}
