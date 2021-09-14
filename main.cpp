#include "MainWindow.h"
#include <QApplication>
#include <iostream>

//#define QAPPLICATION_CLASS QApplication
#include <single_application/SingleApplication>


void test()
{

}

int main(int argc, char* argv[])
{
    test();
    SingleApplication app(argc, argv,true,SingleApplication::Mode::SecondaryNotification,10000);

    QStringList args=app.arguments();
    args.removeAt(0);

    if( app.isSecondary() )
    {
        QByteArray msg=QString(args.join('#')).toLocal8Bit();
//        msg.replace('\\','=');
//        msg.replace(':','+');

        if(!app.sendMessage(msg,10000))
        {
            QMessageBox::information(nullptr,"Error send",QString(msg));
        }
        //QMessageBox::information(nullptr,"Send",QString(msg));

        app.exit( 0 );
    }
    else
    {
        MainWindow w;
        std::cout<<"Start graphy...Version"<<graphyVersion<<std::endl;

        w.show();
        QObject::connect(&app, &SingleApplication::instanceStarted, [ &w ]() {w.raise();w.activateWindow();});
        QObject::connect(&app,&SingleApplication::receivedMessage,&w,&MainWindow::receivedMessage);

        if (args.size()>=1)
        {
            w.direct_open(args);
        }
        else
        {
            std::cout<<args.size()<<std::endl;
        }


        return app.exec();
    }
}
