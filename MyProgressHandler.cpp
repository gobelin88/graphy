#include "MyProgressHandler.h"

MyProgressHandler::MyProgressHandler()
{

}

MyProgressHandler::MyProgressHandler(int max)
{
    reset(max);
}

void MyProgressHandler::reset(int max)
{
    this->what.clear();
    this->max=max;
    this->counter=0;
    this->index=0;

    for(int i=0;i<100;i++)
    {
        indexes[i]=(i*(max-1))/100;
    }
    this->value=indexes[index];
}

void MyProgressHandler::update()
{
    if(counter++==value)
    {
        emit sig_progress(++index);
        if(index<100)
        {
            value=indexes[index];
        }
    }
}

void MyProgressHandler::full()
{
    index=100;
    emit sig_progress(100);
}

bool MyProgressHandler::isBusy()
{
    if(index==100)
    {
        return false;
    }
    std::cout<<"busy="<<index<<std::endl;
    return true;
}

void MyProgressHandler::errorMsg(QString msg)
{
    mutex_what.lock();
    emit sig_error(what,msg);
    mutex_what.unlock();
}

void MyProgressHandler::setWhat(QString what)
{
    mutex_what.lock();
    this->what=what;
    emit sig_what(what);
    mutex_what.unlock();
}

