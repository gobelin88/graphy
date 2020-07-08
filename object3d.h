#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <Qt3DExtras>
#include <Qt3DRender>
#include "qPosAtt.h"

struct Object3D
{
    Object3D(Qt3DCore::QEntity* rootEntity,Qt3DRender::QMesh* m_obj, QPosAtt posatt,float scale,QColor color);

    void setPosAtt(QPosAtt posatt);

    Qt3DCore::QTransform* transform;
    Qt3DExtras::QPhongMaterial* material;
    Qt3DCore::QEntity* entity;
};

#endif // OBJECT3D_H
