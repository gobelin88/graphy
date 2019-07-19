#include "qPosAtt.h"

bool loadPosAttList(QString filename,QPosAttList & list)
{
    if(filename.isEmpty())return false;

    QFile file(filename);
    if(file.open(QIODevice::Text | QIODevice::ReadOnly))
    {
        list.clear();
        while(!file.atEnd())
        {
            QStringList line=QString(file.readLine()).split(";",QString::SkipEmptyParts);

            if(line.size()>=7)
            {

                Eigen::Quaterniond q(line[3].toDouble(),line[4].toDouble(),line[5].toDouble(),line[6].toDouble());
                q.normalize();

                QPosAtt new_pos_att(Eigen::Vector3d(line[0].toDouble(),line[1].toDouble(),line[2].toDouble()),q);

                    list.push_back(new_pos_att);
            }
            else
            {
                std::cout<<line.size()<<std::endl;
                return false;
            }
        }
        file.close();
        return true;
    }
    else
    {
        return false;
    }
}

Eigen::MatrixXd toEigenMatXd(const QPosList & ptlist)
{
    Eigen::MatrixXd M(ptlist.size(),3);

    for(int i=0;i<ptlist.size();i++)
    {
        M(i,0)=ptlist[i].x();
        M(i,1)=ptlist[i].y();
        M(i,2)=ptlist[i].z();
    }

    return M.transpose();
}

bool loadPosList(QString filename,QPosList & list)
{
    if(filename.isEmpty())return false;

    QFile file(filename);
    if(file.open(QIODevice::Text | QIODevice::ReadOnly))
    {
        list.clear();
        while(!file.atEnd())
        {
            QStringList line=QString(file.readLine()).split(";",QString::SkipEmptyParts);

            if(line.size()>=3)
            {
                Eigen::Vector3d new_pos(line[0].toDouble(),line[1].toDouble(),line[2].toDouble());
                list.push_back(new_pos);
            }
            else
            {
                std::cout<<line.size()<<std::endl;
                return false;
            }
        }
        file.close();
        return true;
    }
    else
    {
        return false;
    }
}

QPosList extractPos(QPosAttList & list)
{
   QPosList listp(list.size());

   for(int i=0;i<list.size();i++)
   {
       listp[i]=list[i].P;
   }

   return listp;
}

