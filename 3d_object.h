#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <Qt3DExtras>
#include <Qt3DRender>
#include "PosAtt.h"
#include "3d_base.h"
#include "3d_objectLoader.h"

class Object3D:public Base3D
{
public:
    Object3D(Qt3DCore::QEntity* rootEntity,Qt3DRender::QMesh* m_obj,Object * objet, PosAtt posatt,float scale,QColor color);

    void setPosAtt(PosAtt posatt);

    Object * objet;
};

#endif // OBJECT3D_H
