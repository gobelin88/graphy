#include "MainWindow.h"
#include <QApplication>
#include <iostream>
#include "solidHarmonics/solidHarmonics.h"

//#define QAPPLICATION_CLASS QApplication
#include <single_application/SingleApplication>

void testA()//3 facons de tourner un vecteur de la base A vers la base B.
{
    Eigen::Vector3d Va(1,2,3),Vb_matrix,Vb_quaternion1,Vb_quaternion2;

    Eigen::Matrix3d A=Eigen::Matrix3d::Identity(),B;
    Eigen::Quaterniond Qab(Eigen::AngleAxisd(M_PI/4.0,Eigen::Vector3d(1,1,1)/sqrt(3.0)));
    Eigen::Matrix3d Mab=Qab.toRotationMatrix();

    B=A*Mab;
    Vb_matrix= Mab * Va;
    Vb_quaternion1= Qab._transformVector(Va);
    Vb_quaternion2= (Qab*Eigen::Quaterniond(0,Va.x(),Va.y(),Va.z())*Qab.conjugate()).vec();

    std::cout<<"Vb_matrix="<<Vb_matrix.transpose()<<std::endl;
    std::cout<<"Vb_quaternion1="<<Vb_quaternion1.transpose()<<std::endl;
    std::cout<<"Vb_quaternion2="<<Vb_quaternion2.transpose()<<std::endl;
}

void testB()//! coeffs on quaternion is x y z w
{
    Eigen::Quaterniond Q(1,2,3,4);

    std::cout<<Q.coeffs().transpose()<<std::endl;

}

int main(int argc, char* argv[])
{
    //testA();
    //testB();
    testHarmonics();

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
        std::cout<<"Starting Graphy Version-"<<graphyVersion<<std::endl;
        MainWindow w;
        w.showMaximized();

        QObject::connect(&app, &SingleApplication::instanceStarted, [ &w ]() {w.raise();w.activateWindow();});
        QObject::connect(&app,&SingleApplication::receivedMessage,&w,&MainWindow::receivedMessage);

        if (args.size()>=1)
        {
            w.direct_open(args);
//            w.setArgs(args);
//            QTimer::singleShot(800, &w, SLOT(direct_open_args()));//hum...800
        }

        return app.exec();
    }
}
