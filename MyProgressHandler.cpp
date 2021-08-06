#include "MyProgressHandler.h"

MyProgressHandler::MyProgressHandler()
{
    index=100;//not busy by construction;
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
    indexes[100]=-1;
}

void MyProgressHandler::update()
{
    if(counter++==indexes[index])
    {
        emit sig_progress(++index);
    }    
}

void MyProgressHandler::full()
{
    index=100;
    emit sig_progress(100);
}

bool MyProgressHandler::isBusy()
{
    return index.load()!=100;
}

void MyProgressHandler::setErrorMsg(QString msg)
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

