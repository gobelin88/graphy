#ifndef CLOUD3D_H
#define CLOUD3D_H

#include <Qt3DExtras>
#include <Qt3DRender>
#include "3d_base.h"
#include "Cloud.h"

class Cloud3D:public Base3D
{
public:
    Cloud3D(Cloud * cloud,Qt3DCore::QEntity* rootEntity);
    ~Cloud3D();

    Qt3DRender::QPointSize* pointSize;
    Qt3DRender::QLineWidth* lineWidth;

    void update(QCPRange scalarFieldRange);

    Cloud * cloud;
};

#endif // CLOUD3D_H
