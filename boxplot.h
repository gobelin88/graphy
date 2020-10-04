#ifndef BoxPlot_h
#define BoxPlot_h

struct Resolution
{
    Resolution()
    {
        this->pX_res=10;
        this->pY_res=10;
    }

    Resolution(unsigned int pX_res,unsigned int pY_res)
    {
        this->pX_res=pX_res;
        this->pY_res=pY_res;
    }

    unsigned int pX_res,pY_res;

    void operator=(const Resolution& copy)
    {
        this->pX_res=copy.pX_res;
        this->pY_res=copy.pY_res;
    }
};

#endif // BOX

