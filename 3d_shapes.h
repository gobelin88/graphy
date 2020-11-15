#ifndef GRID3D_H
#define GRID3D_H

#include "3d_base.h"
#include "ShapeFit.h"

#include <Qt3DExtras>
#include <Qt3DRender>

struct Grid3D:public Base3D
{
    Grid3D(Qt3DCore::QEntity* rootEntity,unsigned int N,QColor color);
    QByteArray getGridBuffer(bool xy_swap,bool xz_swap,bool yz_swap,unsigned int N);


};

struct Plan3D:public Base3D
{
    Plan3D(Qt3DCore::QEntity* rootEntity, Plan* plan, float radius, QColor color);
    QByteArray getBuffer(Plan* plan, float radius);

    Plan* plan;
};

struct Sphere3D:public Base3D
{
    Sphere3D(Qt3DCore::QEntity* rootEntity, Sphere* sphere, QColor color);
    QByteArray getBuffer(Sphere* sphere);

    const int Na=50;
    const int Nb=50;

    Sphere* sphere;
};

#endif // GRID3D_H
