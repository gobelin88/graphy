#include "3d_light.h"

Light3D::Light3D(Qt3DCore::QEntity* rootEntity,QColor color,QVector3D position)
{
    lightEntity_a = new Qt3DCore::QEntity(rootEntity);
    light_a = new Qt3DRender::QPointLight(lightEntity_a);
    light_a->setColor(color);
    light_a->setIntensity(1);
    lightEntity_a->addComponent(light_a);
    lightTransform_a = new Qt3DCore::QTransform(lightEntity_a);
    lightTransform_a->setTranslation(position);
    lightEntity_a->addComponent(lightTransform_a);
}
