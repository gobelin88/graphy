#ifndef CLOUD3D_H
#define CLOUD3D_H

#include <Qt3DExtras>
#include <Qt3DRender>
#include "Base3D.h"
#include "Cloud.h"

class Cloud3D:public Base3D
{
public:
    Cloud3D(Cloud * cloud,Qt3DCore::QEntity* rootEntity);

    Qt3DRender::QPointSize* pointSize;
    Qt3DRender::QLineWidth* lineWidth;

    Cloud * cloud;
};

#endif // CLOUD3D_H
