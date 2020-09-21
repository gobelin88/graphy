#ifndef GRID3D_H
#define GRID3D_H

#include "Base3D.h"
#include "ShapeFit.h"

#include <Qt3DExtras>
#include <Qt3DRender>

struct Grid3D:public Base3D
{
    Grid3D(Qt3DCore::QEntity* rootEntity,unsigned int N,QColor color);
    QByteArray getGridBuffer(bool xy_swap,bool xz_swap,bool yz_swap,unsigned int N);

    Qt3DRender::QBuffer* gridIndexBuffer;
    Qt3DRender::QAttribute* gridIndexAttribute;
    Qt3DExtras::QPhongMaterial* gridMaterial;
};

struct Plan3D:public Base3D
{
    Plan3D(Qt3DCore::QEntity* rootEntity, Plan* plan, float radius, QColor color);
    QByteArray getBuffer(Plan* plan, float radius);

    Qt3DRender::QBuffer* indexBuffer;
    Qt3DRender::QAttribute* indexAttribute;
    Qt3DExtras::QPhongAlphaMaterial* material;

};

struct Sphere3D:public Base3D
{
    Sphere3D(Qt3DCore::QEntity* rootEntity, Sphere* sphere, QColor color);
    QByteArray getBuffer(Sphere* sphere);

    Qt3DRender::QBuffer* indexBuffer;
    Qt3DRender::QAttribute* indexAttribute;
    Qt3DExtras::QPhongAlphaMaterial* material;

    const int Na=50;
    const int Nb=50;
};

#endif // GRID3D_H
