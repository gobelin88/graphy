#ifndef BOX
#define BOX

struct Box
{
    Box()
    {
        this->p0_min=0;
        this->p0_max=0;
        this->p0_res=0;

        this->p1_min=0;
        this->p1_max=0;
        this->p1_res=0;

        this->p2_min=0;
        this->p2_max=0;
        this->p2_res=0;

        this->id1=0;
        this->id2=1;
        this->id3=2;
    }

    Box(double p0_min,double p0_max,double p0_res,
        double p1_min,double p1_max,double p1_res,
        double p2_min,double p2_max,double p2_res,
        int id1,int id2,int id3)
    {
        this->p0_min=p0_min;
        this->p0_max=p0_max;
        this->p0_res=p0_res;

        this->p1_min=p1_min;
        this->p1_max=p1_max;        
        this->p1_res=p1_res;

        this->p2_min=p2_min;
        this->p2_max=p2_max;
        this->p2_res=p2_res;

        this->id1=id1;
        this->id2=id2;
        this->id3=id3;
    }

    double getP0(int i){return (i*(p0_max-p0_min))/p0_res+p0_min;}
    double getP1(int i){return (i*(p1_max-p1_min))/p1_res+p1_min;}
    double getP2(int i){return (i*(p2_max-p2_min))/p2_res+p2_min;}

    double fromP0(double i){return (double)((p0_res*(i-p0_min))/(p0_max-p0_min));}
    double fromP1(double i){return (double)((p1_res*(i-p1_min))/(p1_max-p1_min));}
    double fromP2(double i){return (double)((p2_res*(i-p2_min))/(p2_max-p2_min));}

    double p0_min;
    double p0_max;
    int p0_res;

    double p1_min;
    double p1_max;
    int p1_res;

    double p2_min;
    double p2_max;
    int p2_res;

    int id1,id2,id3;

    void Box::operator=(const Box& copy)
    {
        this->p0_min=copy.p0_min;
        this->p0_max=copy.p0_max;
        this->p0_res=copy.p0_res;

        this->p1_min=copy.p1_min;
        this->p1_max=copy.p1_max;        
        this->p1_res=copy.p1_res;

        this->p2_min=copy.p2_min;
        this->p2_max=copy.p2_max;
        this->p2_res=copy.p2_res;

        this->id1=copy.id1;
        this->id2=copy.id2;
        this->id3=copy.id3;
    }


};

#endif // BOX

