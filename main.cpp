#include "MainWindow.h"
#include <QApplication>
#include <iostream>
#include <single_application/SingleApplication>

int main(int argc, char* argv[])
{
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

////////////////////////////////////////////////////////////////////////////////////////
//#include <QtWidgets>
//#include <QtConcurrent>
//#include <MyVariant.h>
//#include <functional>
//#include <iostream>

//using namespace QtConcurrent;

//int main(int argc, char **argv)
//{
//    QApplication app(argc, argv);

//    const int n = 10;

//    // Prepare the vector.
//    QVector<std::pair<QString,int>> vector;
//    for (int i = 0; i < n; ++i)
//        vector.append(std::pair<QString,int>("1;2;3;4;5;6",i));

//    MatrixXv data(n,6);

//    // Create a progress dialog.
//    QProgressBar pbbar;
//    //dialog.setLabelText(QString("Progressing using %1 thread(s)...").arg(QThread::idealThreadCount()));

//    // Create a QFutureWatcher and connect signals and slots.
//    QFutureWatcher<void> futureWatcher;
//    QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &pbbar, &QProgressBar::reset);
//    //QObject::connect(&pbbar, &QProgressBar::canceled, &futureWatcher, &QFutureWatcher<void>::cancel);
//    QObject::connect(&futureWatcher,  &QFutureWatcher<void>::progressRangeChanged, &pbbar, &QProgressBar::setRange);
//    QObject::connect(&futureWatcher, &QFutureWatcher<void>::progressValueChanged,  &pbbar, &QProgressBar::setValue);

//    // Our function to compute
//    std::function<void(std::pair<QString,int>&)> spin = [&](std::pair<QString,int> &x) {
//        QStringList valueList=x.first.split(';');
//        for(int j=0;j<valueList.size();j++)
//        {
//            data(x.second,j).loadFromString(valueList[j]);
//        }
//    };

//    // Start the computation.
//    futureWatcher.setFuture(QtConcurrent::map(vector, spin));

//    // Display the dialog and start the event loop.
//    pbbar.show();
//    app.exec();

//    futureWatcher.waitForFinished();


//    // Query the future to check if was canceled.
//    qDebug() << "Canceled?" << futureWatcher.future().isCanceled();

//    std::cout << data <<std::endl;
//}
