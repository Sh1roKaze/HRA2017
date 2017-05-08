/// \file main.cpp
/// \author xverny00
/// \brief 

#include "card.h"
#include "mywidget.h"

#include <QApplication>
#include <QMenuBar>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    myWidget *widget = new myWidget();

    widget->show();

    return app.exec();
}
