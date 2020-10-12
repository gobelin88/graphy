#include "obj.h"

Object::Object(QString filename, QPosAtt scale_posatt)
{
    params.resize(7);

    wire.clear();

    QFile file(filename);

    unsigned int index=0;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::cout<<"open object"<<std::endl;
        unsigned int id=0;

        QStringList content=QString(file.readAll()).split("\n",QString::SkipEmptyParts);

        int nbp=0;
        for(int i=0;i<content.size();i++)
        {
            if(content[i].contains("v "))
            {
                nbp++;
            }
        }
        pts.resize(3,nbp);

        for(int i=0;i<content.size();i++)
        {
            QString line_str=content[i];
            id++;
            QStringList line=line_str.split(QRegExp("[ |\n]"),QString::SkipEmptyParts);
            if (line.size()>0)
            {
                if (line[0]=="v" && line.size()==4)
                {
                    pts.col(index)=Vector3d(line[1].toDouble(),line[2].toDouble(),line[3].toDouble());
                    index++;
                }
                else if (line[0]=="f")
                {
                    Face new_face;
                    std::vector<Vector2d> new_face_tex_coord;
                    for (int i=1; i<line.size(); i++)
                    {
                        QStringList args=line[i].split("/");
                        if (args.size()>=1)
                        {
                            int idp=args[0].toInt()-1;
                            if (idp<pts.cols())
                            {
                                new_face.push_back( args[0].toInt()-1 );
                                new_face_tex_coord.push_back( Vector2d (0,0) );
                            }
                            else
                            {
                                new_face.push_back( 0 );
                                new_face_tex_coord.push_back( Vector2d (0,0) );
                                std::cout<<"Invalid index f line : "<<id<<" index="<<idp<<std::endl;
                            }
                        }
                        else
                        {
                            new_face.push_back( 0 );
                            new_face_tex_coord.push_back( Vector2d (0,0) );
                            std::cout<<"Error"<<std::endl;
                        }
                    }
                    faces.push_back(new_face);
                    texCoord.push_back(new_face_tex_coord);
                }
            }
        }
        computeNormals();

        file.close();
        open=true;

        pts_base=pts;

        setScalePosAtt(scale_posatt);
    }
    else
    {
        open=false;
        std::cout<<"Impossible d'ouvrir :"<<filename.toLocal8Bit().data()<<std::endl;
    }
}

void Object::save(QString filename)
{
    QFile file(filename);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream ts(&file);

        for(int i=0;i<pts.cols();i++)
        {
            ts<<"v "<<pts.col(i).x()<<" "<<pts.col(i).y()<<" "<<pts.col(i).z()<<"\n";
        }

        for(int i=0;i<faces.size();i++)
        {
            ts<<"f";
            for(int j=0;j<faces[i].size();j++)
            {
                ts<<" "<<faces[i][j]+1;
            }
            ts<<"\n";
        }

        file.close();
    }

}

void Object::disp()
{
    std::cout<<"points :"<<std::endl;
    for (int i=0; i<pts.cols(); i++)
    {
        std::cout<<pts.col(i).x()<<" "<<pts.col(i).y()<<" "<<pts.col(i).z()<<" "<<std::endl;
    }
    std::cout<<"faces :"<<std::endl;
    for (int i=0; i<faces.size(); i++)
    {
        for (int j=0; j<faces[i].size(); j++)
        {
            std::cout<<faces[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
}

BoundingBox Object::getBox()
{
    Vector3d minP=pts.col(0),maxP=pts.col(0);
    for (int i=1; i<pts.cols(); i++)
    {
        if (pts.col(i).x()>maxP.x())
        {
            maxP.x()=pts.col(i).x();
        }
        if (pts.col(i).x()<minP.x())
        {
            minP.x()=pts.col(i).x();
        }
        if (pts.col(i).y()>maxP.y())
        {
            maxP.y()=pts.col(i).y();
        }
        if (pts.col(i).y()<minP.y())
        {
            minP.y()=pts.col(i).y();
        }
        if (pts.col(i).z()>maxP.z())
        {
            maxP.z()=pts.col(i).z();
        }
        if (pts.col(i).z()<minP.z())
        {
            minP.z()=pts.col(i).z();
        }
    }
    return BoundingBox(minP,maxP);
}

Vector3d Object::getBarycenter()
{
    return pts.colwise().mean();
}

Vector3d Object::nearest(int face_id, const Vector3d& p) const
{
    const Face & face=faces[face_id];
    const Vector3d & N=normals.col(face_id);
    Vector3d P=p-N*N.dot(p-pts.col(face[0]));//projection de P sur le plan de la face
    double dmin=DBL_MAX;
    bool positif=false,inside=true;
    Vector3d pproj_poly;
    unsigned int faces_size=face.size();

    for (unsigned int j=0; j<faces_size; j++)
    {
        unsigned int ind_a=face[j];
        unsigned int ind_b=face[(j+1)%faces_size];
        const Vector3d & A=pts.col(ind_a);
        const Vector3d & B=pts.col(ind_b);

        Vector3d Vap=P-A;
        Vector3d Vab=B-A;

        Vector3d n=Vap.cross(Vab);
        double dot=Vap.dot(Vab);

        //Distance^2 à B-------------------------
        if (dot>Vab.squaredNorm())
        {
            double d=(B-P).squaredNorm();
            if (d<dmin)
            {
                dmin=d;
                pproj_poly=B;//B
            }
        }
        //Distance^2 à A--------------------------
        else if (dot<0)
        {
            double d=Vap.squaredNorm();
            if (d<dmin)
            {
                dmin=d;
                pproj_poly=A;//A
            }
        }
        //Distance^2 à AB-------------------------
        else
        {
            double Vba_norm2=Vab.squaredNorm();
            double d=n.squaredNorm()/Vba_norm2;
            if (d<dmin)
            {
                dmin=d;
                pproj_poly=dot*Vab/sqrt(Vba_norm2);
            }
        }

        if (n.dot(N)>0)
        {
            if (j==0)
            {
                positif=true;
            }
            else if (positif==false)
            {
                inside=false;
            }
        }
        else
        {
            if (j==0)
            {
                positif=false;
            }
            else if (positif==true)
            {
                inside=false;
            }
        }
    }

    if (inside)
    {
        return P;
    }
    else
    {
        return pproj_poly;
    }
}

int Object::nb_params()
{
    return params.size();
}

void Object::setParams(const Eigen::VectorXd& p)
{
    this->params=p;

    Q_transform.coeffs()[0]=params[3];
    Q_transform.coeffs()[1]=params[4];
    Q_transform.coeffs()[2]=params[5];
    Q_transform.coeffs()[3]=params[6];
    P_transform[0]         =params[0];
    P_transform[1]         =params[1];
    P_transform[2]         =params[2];

    Q_norm=Q_transform.norm();
    Quaterniond Q_transform_normed=Q_transform;
    Q_transform_normed.coeffs()/=Q_norm;
    R_transform.noalias()=Q_transform_normed.toRotationMatrix();

    pts.noalias()    =(R_transform*Q_norm)*pts_base; //p'=Rp+P
    pts.colwise()   +=P_transform;
    normals.noalias()=R_transform*normals_base;
}

void Object::setScalePosAtt(const QPosAtt& scalePosatt)
{
    params<<scalePosatt.P,scalePosatt.Q.coeffs();
    setParams(params);
}

QPosAtt Object::getPosAtt()
{
    return QPosAtt(P_transform,Q_transform);
}

double Object::getScale()
{
    return Q_norm;
}

const Eigen::VectorXd& Object::getParams()
{
    return params;
}

Vector3d Object::nearestDelta(const Vector3d& p) const
{
    double dmin=DBL_MAX;
    Vector3d nearestDelta;
    for (int i=0; i<faces.size(); i++)
    {
        Vector3d delta=p-nearest(i,p);
        double distance=delta.squaredNorm();

        if(distance<dmin)
        {
            dmin=distance;
            nearestDelta=delta;
        }
    }
    return nearestDelta;
}

Vector3d Object::delta(const Vector3d& p) const
{
    return nearestDelta(p);
}

double Object::getRadius()
{
    Vector3d bary=getBox().middle();
    double radius=0.0;
    for (int i=0; i<pts.cols(); i++)
    {
        radius+=(pts.col(i)-bary).norm();
    }
    return radius/pts.cols();
}

void Object::computeNormals()
{
    //normals_base.resize(faces.size());
    normals_base.resize(3,faces.size());
    for (int i=0; i<faces.size(); i++)
    {
        if (faces[i].size()>=3)
        {
            normals_base.col(i)=getNormal(faces[i]);

//            if (N.dot(getBarycenter(faces[i]))>0)
//            {
//                normals.push_back(N);
//            }
//            else
//            {
//                normals.push_back(-N);
//            }
        }
        else
        {
            normals_base.col(i)=Vector3d(0,1,0);
        }
    }
    normals=normals_base;
}

Vector3d Object::getNormal(const Face& f)const
{
    Vector3d n=(pts.col(f[1])-pts.col(f[0])).cross(pts.col(f[2])-pts.col(f[0]));
    return n/n.norm();
}

Base Object::getBase(const Face& f)const
{
    Base b;

    if (f.size()>=3)
    {
        Vector3d pa=pts.col(f[0]),pb=pts.col(f[1]),pc=pts.col(f[2]);

        b.col(0)=(pb-pa).normalized();
        b.col(1)=( (pc-pa)-(pc-pa).dot(b.col(0))*b.col(0) ).normalized() ;
        b.col(2)=b.col(1).cross(b.col(0));
    }

    return b;
}

Vector2d Object::getCoord2D(const Base& b,Vector3d p,Vector3d bary)const
{
    return Vector2d( b.col(0).dot(p-bary), b.col(1).dot(p-bary) );
}

Vector3d Object::getCoord3D(const Base& b,Vector2d p,Vector3d bary)const
{
    return bary+b.col(0)*p.x()+b.col(1)*p.y();
}

Vector3d Object::getBarycenter(const Face& f)const
{
    Vector3d bary(0,0,0);
    for (int i=0; i<f.size(); i++)
    {
        bary+=pts.col(f[i]);
    }
    return bary/f.size();
}

Vector2d clampSegment(Vector2d p,Vector2d A,Vector2d B,Vector2d U, bool& ok)
{
    Vector2d V=B-A;
    Matrix2d M;
    M.col(0)=V;
    M.col(1)=-U;

    Vector2d K=M.jacobiSvd(Eigen::ComputeFullU | Eigen::ComputeFullV).solve(p-A);
    //Vector2d K=M.inverse()*(p-A);

    if (K[0]>=0 && K[0]<=1.0 && std::isnormal(K[0]))
    {
        ok=true;
    }
    else
    {
        ok=false;
    }

    return K[0]*V+A;
}
