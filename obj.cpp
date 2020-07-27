#include "obj.h"

Object::Object(QString filename, double scale, QPosAtt posatt)
{
    params.resize(8);

    wire.clear();

    QFile file(filename);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::cout<<"open object"<<std::endl;
        unsigned int id=0;
        while (!file.atEnd())
        {
            QString line_str=file.readLine();
            id++;
            QStringList line=line_str.split(QRegExp("[ |\n]"),QString::SkipEmptyParts);
            if (line.size()>0)
            {
                if (line[0]=="v" && line.size()==4)
                {
                    pts.push_back(Vector3d(line[1].toDouble(),line[2].toDouble(),line[3].toDouble()));
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
                            if (idp<pts.size())
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

        setScalePosAtt(scale,posatt);
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

        for(int i=0;i<pts.size();i++)
        {
            ts<<"v "<<pts[i].x()<<" "<<pts[i].y()<<" "<<pts[i].z()<<"\n";
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
    for (int i=0; i<pts.size(); i++)
    {
        std::cout<<pts[i].x()<<" "<<pts[i].y()<<" "<<pts[i].z()<<" "<<std::endl;
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
    Vector3d minP=pts[0],maxP=pts[0];
    for (int i=1; i<pts.size(); i++)
    {
        if (pts[i].x()>maxP.x())
        {
            maxP.x()=pts[i].x();
        }
        if (pts[i].x()<minP.x())
        {
            minP.x()=pts[i].x();
        }
        if (pts[i].y()>maxP.y())
        {
            maxP.y()=pts[i].y();
        }
        if (pts[i].y()<minP.y())
        {
            minP.y()=pts[i].y();
        }
        if (pts[i].z()>maxP.z())
        {
            maxP.z()=pts[i].z();
        }
        if (pts[i].z()<minP.z())
        {
            minP.z()=pts[i].z();
        }
    }
    return BoundingBox(minP,maxP);
}

Vector3d Object::getBarycenter()
{
    Vector3d bary(0,0,0);
    for (int i=0; i<pts.size(); i++)
    {
        bary+=pts[i];
    }
    return bary/pts.size();
}

Vector3d Object::nearest(int face_id, const Vector3d& p) const
{
    Face face=faces[face_id];
    Vector3d N=getBase(face).col(2);
    Vector3d Pp=p-N*N.dot(p-pts[face[0]]);
    double dmin=DBL_MAX;
    bool positif=false,inside=true;
    Vector3d pproj_poly;

    int nb_faces=face.size();

    for (int j=0; j<nb_faces; j++)
    {
        Vector3d Pa=pts[face[j]];
        Vector3d Pb=pts[face[(j+1)%nb_faces]];
        Vector3d Vpa=Pp-Pa;
        Vector3d Vba=Pb-Pa;
        double Vba_norm=Vba.norm();
        Vector3d Vba_normed=Vba/Vba_norm;
        Vector3d n=Vpa.cross(Vba_normed);

        double dot=Vba_normed.dot(Vpa);

        if (dot>Vba_norm)
        {
            double d=(Pb-Pp).norm();
            if (d<dmin)
            {
                dmin=d;
                pproj_poly=Pb;
            }
        }
        else if (dot<0)
        {
            double d=Vpa.norm();
            if (d<dmin)
            {
                dmin=d;
                pproj_poly=Pa;
            }
        }
        else
        {
            double d=n.norm();
            if (d<dmin)
            {
                dmin=d;
                pproj_poly=dot*Vba_normed;
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
        return Pp;
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
    this->mat=getPosAtt().toMatrice(p[0]);
    transform();
}

Vector3d Object::transform(const Vector3d& p)
{
    Eigen::Vector4d tp=mat*Eigen::Vector4d (p[0],p[1],p[2],1.0);
    return Vector3d(tp[0],tp[1],tp[2]);
}

void Object::transform()
{
    for (int i=0; i<pts_base.size(); i++)
    {
        pts[i]=transform(pts_base[i]);
    }
}

void Object::setScalePosAtt(double scale, const QPosAtt& posatt)
{
    params<<scale,posatt.P,posatt.Q.coeffs();
    setParams(params);
}

QPosAtt Object::getPosAtt()
{
    Eigen::Quaterniond q(params[4],params[5],params[6],params[7]);
    q.normalize();
    return QPosAtt(Vector3d(params[1],params[2],params[3]),q);
}

double Object::getScale()
{
    return params[0];
}

const Eigen::VectorXd& Object::getParams()
{
    return params;
}

Vector3d Object::nearest(const Vector3d& p) const
{
    Vector3d p_nearest;
    std::vector<Vector3d> projected_p(faces.size());
    VectorXd distances(faces.size());

    //#pragma omp parallel for
    for (int i=0; i<faces.size(); i++)
    {
        projected_p[i]=nearest(i,p);
        distances[i]=(p-projected_p[i]).squaredNorm();
    }

    unsigned int minIndex;
    distances.minCoeff(&minIndex);
    return projected_p[minIndex];
}

Vector3d Object::delta(const Vector3d& p) const
{
    return p-nearest(p);
}

double Object::getRadius()
{
    Vector3d bary=getBox().middle();
    double radius=0.0;
    for (int i=0; i<pts.size(); i++)
    {
        radius+=(pts[i]-bary).norm();
    }
    return radius/pts.size();
}

void Object::computeNormals()
{
    for (int i=0; i<faces.size(); i++)
    {
        if (faces[i].size()>=3)
        {
            Vector3d N=getBase(faces[i]).col(2);

            if (N.dot(getBarycenter(faces[i]))>0)
            {
                normals.push_back(N);
            }
            else
            {
                normals.push_back(-N);
            }
        }
        else
        {
            normals.push_back(Vector3d(0,1,0));
        }
    }
}

Base Object::getBase(const Face& f)const
{
    Base b;

    if (f.size()>=3)
    {
        Vector3d pa=pts[f[0]],pb=pts[f[1]],pc=pts[f[2]];

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
        bary+=pts[f[i]];
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
