#include "MainWindow.h"
#include <QApplication>
#include <iostream>

int main(int argc, char* argv[])
{
//    Eigen::Quaterniond q;

//    for(int i=0;i<4;i++)
//    {
//        q=Eigen::Quaterniond( Eigen::AngleAxisd((i-1.5)*M_PI/6, Eigen::Vector3d::UnitX()) );
//        std::cout<<q.w()<<";"<<q.x()<<";"<<q.y()<<";"<<q.z()<<std::endl;
//    }

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
