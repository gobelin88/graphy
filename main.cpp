#include "mainwindow.h"
#include <QApplication>
#include <iostream>

int main(int argc, char* argv[])
{
    std::cout<<"ok"<<std::endl;

    QApplication a(argc, argv);

    QStringList args=a.arguments();

    MainWindow w;
    if (args.size()==2)
    {
        w.direct_open(args[1]);
    }
    else
    {
        std::cout<<args.size()<<std::endl;
    }

    w.showMaximized();
    return a.exec();
}
