#ifndef BoxPlot_h
#define BoxPlot_h

struct BoxPlot
{
    BoxPlot()
    {
        this->pX_res=10;
        this->pY_res=10;

        this->idX=0;
        this->idY=1;
        this->idZ=2;
    }

    BoxPlot(unsigned int pX_res,unsigned int pY_res,
        unsigned int idX,unsigned int idY,unsigned int idZ)
    {
        this->pX_res=pX_res;
        this->pY_res=pY_res;

        this->idX=idX;
        this->idY=idY;
        this->idZ=idZ;
    }

    unsigned int pX_res,pY_res;
    unsigned int idX,idY,idZ;

    void operator=(const BoxPlot& copy)
    {
        this->pX_res=copy.pX_res;
        this->pY_res=copy.pY_res;

        this->idX=copy.idX;
        this->idY=copy.idY;
        this->idZ=copy.idZ;
    }
};

#endif // BOX

