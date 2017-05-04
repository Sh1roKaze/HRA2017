#include "card.h"
#include "mywidget.h"

#include <QApplication>
//#include <QGraphicsScene>
//#include <QGraphicsView>
//#include <QGridLayout>
#include <QMenuBar>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    myWidget *widget = new myWidget();

    widget->show();

    return app.exec();
}
