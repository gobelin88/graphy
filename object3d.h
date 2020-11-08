#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <Qt3DExtras>
#include <Qt3DRender>
#include "qPosAtt.h"
#include "Base3D.h"

class Object3D:public Base3D
{
public:
    Object3D(Qt3DCore::QEntity* rootEntity,Qt3DRender::QMesh* m_obj, QPosAtt posatt,float scale,QColor color);

    void setPosAtt(QPosAtt posatt);

    Qt3DExtras::QPhongMaterial* material;
};

#endif // OBJECT3D_H
