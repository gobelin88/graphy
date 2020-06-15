#ifndef CLOUD3D_H
#define CLOUD3D_H

#include <Qt3DExtras>
#include <Qt3DRender>
#include "Base3D.h"

struct Cloud3D:public Base3D
{
    Cloud3D(Qt3DCore::QEntity* rootEntity);

    Qt3DExtras::QPerVertexColorMaterial* cloudMaterial;
    Qt3DRender::QPointSize* pointSize;
    Qt3DRender::QLineWidth* lineWidth;
    Qt3DCore::QTransform* cloudTransform;
    Qt3DRender::QAttribute* cloudColorsAttribute;
};

#endif // CLOUD3D_H
