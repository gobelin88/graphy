#ifndef GRID3D_H
#define GRID3D_H

#include "3d_base.h"
#include "shapes/Sphere.hpp"
#include "shapes/Ellipsoid.hpp"
#include "shapes/Plan.hpp"

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
    ~Plan3D();
    QByteArray getBuffer(float radius);

    Plan* plan;
};

struct Sphere3D:public Base3D
{
    Sphere3D(Qt3DCore::QEntity* rootEntity, Sphere* sphere, QColor color);
    ~Sphere3D();
    QByteArray getBuffer();

    const int Na=50;
    const int Nb=50;

    Sphere* sphere;
};


struct Ellipsoid3D:public Base3D
{
    Ellipsoid3D(Qt3DCore::QEntity* rootEntity, Ellipsoid* ellipsoid, QColor color);
    ~Ellipsoid3D();
    QByteArray getBuffer();

    const int Na=50;
    const int Nb=50;

    Ellipsoid* ellipsoid;
};
#endif // GRID3D_H
