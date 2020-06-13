#ifndef CAMERAPARAMS_H
#define CAMERAPARAMS_H

#include <Qt3DExtras>
#include <Qt3DRender>

struct CameraParams
{
    CameraParams(Qt3DRender::QCamera* cameraEntity,float alpha,float beta,float radius,QVector3D barycenter=QVector3D(0,0,0),float boundingRadius=0.0);

    void init(float alpha,float beta,float radius);
    void update();
    void reset();

    void move(float dx,float dy);
    void moveTo(float alpha,float beta,float radius);
    void setBarycenter(QVector3D barycenter);
    QVector3D getBarycenter();
    void setBoundingRadius(float boundingRadius);
    float getBoundingRadius();
    float getRadius();
    void setRadius(float radius);

    Qt3DRender::QCamera* entity();

    double getAlpha();
    double getBeta();

private:
    float alpha,beta;
    float radius;
    float boundingRadius;
    QVector3D barycenter;
    Qt3DRender::QCamera* cameraEntity;
};

#endif // CAMERAPARAMS_H
