#ifndef GRID3D_H
#define GRID3D_H

#include "Base3D.h"

#include <Qt3DExtras>
#include <Qt3DRender>

struct Grid3D:public Base3D
{
    Grid3D(Qt3DCore::QEntity* rootEntity,unsigned int N,QColor color);
    QByteArray getGridBuffer(bool xy_swap,bool xz_swap,bool yz_swap,int N);

    Qt3DRender::QBuffer* gridIndexBuffer;
    Qt3DRender::QAttribute* gridIndexAttribute;
    Qt3DExtras::QPhongMaterial* gridMaterial;
};

#endif // GRID3D_H
