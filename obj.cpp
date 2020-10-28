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

void Object::exportEdges(QString filename)
{
    QFile file(filename);

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream ts(&file);
        ts<<"<header>\n";
        ts<<"X;Y;Z;S\n";
        ts<<"</header>\n";
        for(int i=0;i<edges.size();i++)
        {
            Vector3d A=pts.col(edges[i][0]);
            Vector3d B=pts.col(edges[i][1]);

            ts<<A[0]<<";"<<A[1]<<";"<<A[2]<<";"<<edges[i].value<<";\n";
            ts<<B[0]<<";"<<B[1]<<";"<<B[2]<<";"<<edges[i].value<<";\n";
        }
        file.close();
    }
}

void Object::subdiviseEdges()
{
    int size=edges.size();
    for(int i=0;i<size;i++)
    {
        int ida=edges[i][0];
        int idb=edges[i][1];
        Vector3d A=pts.col(ida);
        Vector3d B=pts.col(idb);
        Vector3d V=B-A;
        Vector3d M=A+V*0.5;

        dataAddColumn(pts,M);
        edges[i][1]=pts.cols()-1;

        edges.push_back(Edge(edges[i][1],idb,edges[i].value));
    }
    //pts=pts_base;
}

void Object::project(Shape<Eigen::Vector3d>* model)
{
    for(int i=0;i<pts.cols();i++)
    {
        pts.col(i)-=model->delta(pts.col(i));
    }
}

void Object::projectOnBoundingSphere(Vector3d center)
{
    Sphere sphere(center,getRadius(center));
    project(&sphere);
}

void Object::rotegrity(double angle,
                       int nbsub,
                       double strech,
                       double radius_int,
                       double radius_dr,
                       double width,
                       double delta_ext,
                       double delta_int)
{
    computeEdges();
    cutEdges();

    //rotate all segments
    Vector3d center(0,0,0);
    for(int i=0;i<edges.size();i++)
    {
        Vector3d A=pts.col(edges[i][0]);
        Vector3d B=pts.col(edges[i][1]);

        Vector3d V=B-A;
        Vector3d M=A+V*0.5;
        Vector3d U=(M-center)/(M-center).norm();//Axe de rotation

        AngleAxisd angleAxis(angle*M_PI/180.0,U);

        pts.col(edges[i][0])=M+angleAxis._transformVector((A-M)*strech);
        pts.col(edges[i][1])=M+angleAxis._transformVector((B-M)*strech);
        edges[i].value=V.norm();
    }

    double radius_ext=radius_int+radius_dr;
    faces.clear();
    for(int i=0;i<edges.size();i++)
    {
        Vector3d A=pts.col(edges[i][0]);
        Vector3d B=pts.col(edges[i][1]);
        Vector3d V=B-A;

        Vector3d N=V.cross(A);
        N=N/N.norm();

        groups.push_back(faces.size());

        Vector3d Ap =A /A .norm()*(radius_ext+radius_int)*0.5;
        Vector3d Bp =B /B .norm()*(radius_ext+radius_int)*0.5;

        double delta_ext_size=width/(Bp-Ap).norm()*sqrt(2)*1.24;
        double delta_int_size=width/(Bp-Ap).norm()*sqrt(2);

        std::cout<<delta_ext_size<<" "<<delta_ext_size<<std::endl;

        for(int j=0;j<nbsub;j++)
        {
            double alpha=double(j+0.5)/nbsub;
            Vector3d P =V/nbsub*(j)  +A;
            Vector3d Pp=V/nbsub*(j+1)+A;

            double dr_ext=0.0;
            if( (alpha>delta_ext && alpha<(delta_ext+delta_ext_size)) ||
                (alpha<(1-delta_ext) && alpha>(1-(delta_ext+delta_ext_size))))
            {
                dr_ext=-radius_dr/2;
            }

            double dr_int=0.0;
            if( (alpha>delta_int && alpha<(delta_int+delta_int_size)) ||
                (alpha<(1-delta_int) && alpha>(1-(delta_int+delta_int_size))))
            {
                dr_int=radius_dr/2;
            }


            Vector3d Pa1 =P /P .norm()*(radius_ext+dr_ext) +N*width*0.5;//A-->B
            Vector3d Pb1 =P /P .norm()*(radius_int+dr_int)+N*width*0.5;//B-->A
            Vector3d Pap1=Pp/Pp.norm()*(radius_ext+dr_ext) +N*width*0.5;//A-->B
            Vector3d Pbp1=Pp/Pp.norm()*(radius_int+dr_int)+N*width*0.5;//B-->A

            Vector3d Pa2 =P /P .norm()*(radius_ext+dr_ext) -N*width*0.5;//A-->B
            Vector3d Pb2 =P /P .norm()*(radius_int+dr_int)-N*width*0.5;//B-->A
            Vector3d Pap2=Pp/Pp.norm()*(radius_ext+dr_ext) -N*width*0.5;//A-->B
            Vector3d Pbp2=Pp/Pp.norm()*(radius_int+dr_int)-N*width*0.5;//B-->A

            Face new_face1;
            Face new_face2;
            Face new_face3;
            Face new_face4;

            int id1[4],id2[4];

            dataAddColumn(pts,Pa1) ;id1[0]=pts.cols()-1;
            dataAddColumn(pts,Pb1) ;id1[1]=pts.cols()-1;
            dataAddColumn(pts,Pbp1);id1[2]=pts.cols()-1;
            dataAddColumn(pts,Pap1);id1[3]=pts.cols()-1;

            dataAddColumn(pts,Pa2) ;id2[0]=pts.cols()-1;
            dataAddColumn(pts,Pb2) ;id2[1]=pts.cols()-1;
            dataAddColumn(pts,Pbp2);id2[2]=pts.cols()-1;
            dataAddColumn(pts,Pap2);id2[3]=pts.cols()-1;

            new_face1.push_back(id1[0]);
            new_face1.push_back(id1[1]);
            new_face1.push_back(id1[2]);
            new_face1.push_back(id1[3]);

            new_face2.push_back(id2[0]);
            new_face2.push_back(id2[1]);
            new_face2.push_back(id2[2]);
            new_face2.push_back(id2[3]);

            new_face3.push_back(id1[0]);
            new_face3.push_back(id1[3]);
            new_face3.push_back(id2[3]);
            new_face3.push_back(id2[0]);

            new_face4.push_back(id1[1]);
            new_face4.push_back(id1[2]);
            new_face4.push_back(id2[2]);
            new_face4.push_back(id2[1]);

            faces.push_back(new_face1);
            faces.push_back(new_face2);
            faces.push_back(new_face3);
            faces.push_back(new_face4);

            if(j==0)
            {
                Face new_face5;
                new_face5.push_back(id1[0]);
                new_face5.push_back(id1[1]);
                new_face5.push_back(id2[1]);
                new_face5.push_back(id2[0]);
                faces.push_back(new_face5);
            }

            if(j==nbsub-1)
            {
                Face new_face5;
                new_face5.push_back(id1[2]);
                new_face5.push_back(id1[3]);
                new_face5.push_back(id2[3]);
                new_face5.push_back(id2[2]);
                faces.push_back(new_face5);
            }

        }

    }
    computeNormals();
}

bool Object::isNewGroup(int i)
{
    std::vector<int>::iterator it=std::find(groups.begin(),groups.end(),i);

    if (it != groups.end())
    {
        return true;
    }
    else
    {
        return false;
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

        for(int i=0;i<normals.cols();i++)
        {
            ts<<"vn "<<normals.col(i).x()<<" "<<normals.col(i).y()<<" "<<normals.col(i).z()<<"\n";
        }

        if(normals.cols()==faces.size())
        {
            for(int i=0;i<faces.size();i++)
            {
                if(isNewGroup(i))
                {
                    ts<<"g "<<i<<"\n";
                }
                ts<<"f";
                for(int j=0;j<faces[i].size();j++)
                {
                    ts<<" "<<faces[i][j]+1<<"//"<<i;
                }
                ts<<"\n";
            }
        }
        else
        {
            for(int i=0;i<faces.size();i++)
            {
                if(isNewGroup(i))
                {
                    ts<<"g "<<i<<"\n";
                }
                ts<<"f";
                for(int j=0;j<faces[i].size();j++)
                {
                    ts<<" "<<faces[i][j]+1;
                }
                ts<<"\n";
            }
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

void Object::nearestPolygon(int face_id, const Vector3d& p, double *r) const
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
        r[0]=P[0];
        r[1]=P[1];
        r[2]=P[2];
    }
    else
    {
        r[0]=pproj_poly[0];
        r[1]=pproj_poly[1];
        r[2]=pproj_poly[2];
    }
}


inline double clampd(double d) {
  const double t = d < 0.0 ? 0.0 : d;
  return t > 1.0 ? 1.0 : t;
}

void Object::nearestTriangle(int face_id, const Vector3d& p ,double * r )const
{
    const Face & face=faces[face_id];
    const double * P=p.data();
    const double * A=pts.col(face[0]).data();
    const double * B=pts.col(face[1]).data();
    const double * C=pts.col(face[2]).data();

    double AB[3]={B[0] - A[0],
                     B[1] - A[1],
                     B[2] - A[2]};

    double AC[3]={C[0] - A[0],
                     C[1] - A[1],
                     C[2] - A[2]};

    double PA[3] ={ A[0] - P[0],
                    A[1] - P[1],
                    A[2] - P[2]};

    double a = AB[0]*AB[0]+AB[1]*AB[1]+AB[2]*AB[2];
    double b = AB[0]*AC[0]+AB[1]*AC[1]+AB[2]*AC[2];
    double c = AC[0]*AC[0]+AC[1]*AC[1]+AC[2]*AC[2];
    double d = AB[0]*PA[0]+AB[1]*PA[1]+AB[2]*PA[2];
    double e = AC[0]*PA[0]+AC[1]*PA[1]+AC[2]*PA[2];

    double det = a*c - b*b;
    double s = b*e - c*d;
    double t = b*d - a*e;

    if ( s + t < det )
    {
        if ( s < 0.f )
        {
            if ( t < 0.f )
            {
                if ( d < 0.f )
                {
                    s = clampd( -d/a);
                    t = 0.f;

                    r[0]= A[0] + s * AB[0];
                    r[1]= A[1] + s * AB[1];
                    r[2]= A[2] + s * AB[2];
                    return;
                }
                else
                {
                    s = 0.f;
                    t = clampd( -e/c);

                    r[0]= A[0] + t * AC[0];
                    r[1]= A[1] + t * AC[1];
                    r[2]= A[2] + t * AC[2];
                    return;
                }
            }
            else
            {
                s = 0.f;
                t = clampd( -e/c);

                r[0]= A[0] + t * AC[0];
                r[1]= A[1] + t * AC[1];
                r[2]= A[2] + t * AC[2];
                return;
            }
        }
        else if ( t < 0.f )
        {
            s = clampd( -d/a);
            t = 0.f;

            r[0]= A[0] + s * AB[0];
            r[1]= A[1] + s * AB[1];
            r[2]= A[2] + s * AB[2];
            return;
        }
        else
        {
            double invDet = 1.f / det;
            s *= invDet;
            t *= invDet;
        }
    }
    else
    {
        if ( s < 0.f )
        {
            double tmp0 = b+d;
            double tmp1 = c+e;
            if ( tmp1 > tmp0 )
            {
                double numer = tmp1 - tmp0;
                double denom = a-2*b+c;
                s = clampd( numer/denom );
                t = 1-s;
            }
            else
            {
                t = clampd( -e/c);
                s = 0.f;

                r[0]= A[0] + t * AC[0];
                r[1]= A[1] + t * AC[1];
                r[2]= A[2] + t * AC[2];
                return;
            }
        }
        else if ( t < 0.f )
        {
            if ( a+d > b+e )
            {
                double numer = c+e-b-d;
                double denom = a-2*b+c;
                s = clampd( numer/denom );
                t = 1-s;
            }
            else
            {
                s = clampd( -e/c );
                t = 0.f;

                r[0]= A[0] + s * AB[0];
                r[1]= A[1] + s * AB[1];
                r[2]= A[2] + s * AB[2];
                return;
            }
        }
        else
        {
            double numer = c+e-b-d;
            double denom = a-2*b+c;
            s = clampd( numer/denom );
            t = 1.f - s;
        }
    }


    r[0]= A[0] + s * AB[0] + t * AC[0];
    r[1]= A[1] + s * AB[1] + t * AC[1];
    r[2]= A[2] + s * AB[2] + t * AC[2];

}

double Object::nearestDelta(int face_id, const Vector3d& p,Vector3d & delta) const
{
    double r[3];

    if(faces[face_id].size()==3)
    {
        nearestTriangle(face_id,p,r);
    }
    else
    {
        nearestPolygon(face_id,p,r);
    }

    delta[0]=p[0]-r[0];
    delta[1]=p[1]-r[1];
    delta[2]=p[2]-r[2];

    return delta.squaredNorm();
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

Vector3d Object::delta(const Vector3d& p) const
{
    double dmin=DBL_MAX;
    Vector3d nDelta,delta;
    for (int i=0; i<faces.size(); i++)
    {
        double distance=nearestDelta(i,p,delta);

        if(distance<dmin)
        {
            dmin=distance;
            nDelta=delta;
        }
    }
    return nDelta;
}

double Object::getRadius(Vector3d center)
{
    double radius=0.0;
    for (int i=0; i<pts.cols(); i++)
    {
        double r=(pts.col(i)-center).norm();
        if(r>radius)
        {
            radius=r;
        }
    }
    return radius;
}

bool Object::isNewEdge(const Edge & edge)
{
    for(int i=0;i<edges.size();i++)
    {
        if((edges[i][0]==edge[0]) && (edges[i][1]==edge[1]))
        {
            return false;
        }
        else if((edges[i][0]==edge[1]) && (edges[i][1]==edge[0]))
        {
            return false;
        }
    }
    return true;
}

void Object::computeEdges()
{
    edges.clear();
    for (int i=0; i<faces.size(); i++)
    {
        for (int a=0; a<faces[i].size(); a++)
        {
            int b=(a+1)%faces[i].size();
            Edge edge(faces[i][a],faces[i][b]);

            if(isNewEdge(edge))
            {
                edges.push_back(edge);
            }

        }
    }
}

void Object::dataAddColumn(Matrix<double,3,Eigen::Dynamic> & matrix, Vector3d colToAdd)
{
    unsigned int numRows = matrix.rows();
    unsigned int numCols = matrix.cols()+1;
    if(numRows==colToAdd.rows())
    {
        matrix.conservativeResize(numRows,numCols);
        matrix.col(numCols-1)=colToAdd;
    }
}

void Object::cutEdge(int id_edge)
{
    int ida=edges[id_edge][0];
    int idb=edges[id_edge][1];

    for(int i=0;i<edges.size();i++)
    {
        if(i!=id_edge)
        {
            for(int j=0;j<2;j++)
            {
                if( (ida==edges[i][j]) || (idb==edges[i][j]) )
                {
                    dataAddColumn(pts,pts.col(edges[i][j]));
                    edges[i][j]=pts.cols()-1;
                }
            }
        }
    }
}

void Object::dispEdges()
{
    std::cout<<"-------------pts.cols()="<<pts.cols()<<std::endl;
    for(int i=0;i<edges.size();i++)
    {
        std::cout<<edges[i].transpose()<<" "<<pts.col(edges[i][0]).transpose()<<" "<<pts.col(edges[i][1]).transpose()<<std::endl;
    }
    std::cout<<"-------------"<<std::endl;
}

void Object::cutEdges()
{
    for(int i=0;i<edges.size();i++)
    {
        cutEdge(i);
    }
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
