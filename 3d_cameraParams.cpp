#include "3d_cameraParams.h"

CameraParams::CameraParams(Qt3DRender::QCamera* cameraEntity,float alpha,float beta,float radius,QVector3D barycenter,float boundingRadius)
{
    this->cameraEntity=cameraEntity;
    init(alpha,beta,radius);
    this->barycenter=barycenter;
    this->boundingRadius=boundingRadius;
}

void CameraParams::init(float _alpha,float _beta,float _radius)
{
    this->alpha=_alpha;
    this->beta=_beta;
    this->radius=_radius;
}

void CameraParams::update()
{
    cameraEntity->setPosition(barycenter+QVector3D(-radius*cos(alpha)*cos(beta),-radius*sin(beta),-radius*sin(alpha)*cos(beta)));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(barycenter);
}

void CameraParams::move(float dx,float dy)
{
    float speed =0.01f,eps=1.0e-2f;
    this->alpha-=dx*speed;

    this->beta+=dy*speed;

    if (beta>(float(M_PI)/2.0f-eps))
    {
        beta=(float(M_PI)/2.0f-eps);
    }
    else if (beta<(-float(M_PI)/2.0f+eps))
    {
        beta=(-float(M_PI)/2.0f+eps);
    }

    update();
}

void CameraParams::moveTo(float _alpha,float _beta,float _radius)
{
    init(_alpha,_beta,_radius);
    update();
}

void CameraParams::setBarycenter(QVector3D _barycenter)
{
    this->barycenter=_barycenter;
    update();
}

QVector3D CameraParams::getBarycenter()
{
    return barycenter;
}

void CameraParams::setBoundingRadius(float _boundingRadius)
{
    this->boundingRadius=_boundingRadius;
}

float CameraParams::getBoundingRadius()
{
    return boundingRadius;
}

float CameraParams::getRadius()
{
    return radius;
}

void CameraParams::setRadius(float _radius)
{
    this->radius=_radius;
    update();
}

void CameraParams::reset()
{
    moveTo(float(M_PI*0.25+M_PI),-float(M_PI*0.10),4*boundingRadius);
}

Qt3DRender::QCamera* CameraParams::entity()
{
    return cameraEntity;
}

float CameraParams::getAlpha()
{
    return alpha;
}
float CameraParams::getBeta()
{
    return beta;
}
