#include "object3d.h"

Object3D::Object3D(Qt3DCore::QEntity* rootEntity,Qt3DRender::QMesh* m_obj, QPosAtt posatt,float scale,QColor color)
{
    t_obj = new Qt3DCore::QTransform();
    t_obj->setScale(scale);
    t_obj->setRotation(toQQuaternion(posatt.Q));
    t_obj->setTranslation(toQVector3D(posatt.P));

    mat_obj = new Qt3DExtras::QPhongMaterial();
    mat_obj->setDiffuse(color);

    //    Qt3DExtras::QPerVertexColorMaterial * mat_obj = new Qt3DExtras::QPerVertexColorMaterial();

    m_objEntity = new Qt3DCore::QEntity(rootEntity);
    m_objEntity->addComponent(m_obj);
    m_objEntity->addComponent(mat_obj);
    m_objEntity->addComponent(t_obj);
}

void Object3D::setPosAtt(QPosAtt posatt)
{
    t_obj->setTranslation(toQVector3D(posatt.P));
    t_obj->setRotation(toQQuaternion(posatt.Q));
}
