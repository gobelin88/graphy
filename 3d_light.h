#ifndef LIGHT3D_H
#define LIGHT3D_H

#include <Qt3DExtras>
#include <Qt3DRender>

struct Light3D
{
    Light3D(Qt3DCore::QEntity* rootEntity,QColor color,QVector3D position);

    Qt3DCore::QEntity* lightEntity_a;
    Qt3DRender::QPointLight* light_a;
    Qt3DCore::QTransform* lightTransform_a;
};

#endif // LIGHT3D_H
