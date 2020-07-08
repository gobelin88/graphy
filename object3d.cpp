#include "object3d.h"

Object3D::Object3D(Qt3DCore::QEntity* rootEntity,Qt3DRender::QMesh* m_obj, QPosAtt posatt,float scale,QColor color)
{
    transform = new Qt3DCore::QTransform();
    transform->setScale(scale);

    setPosAtt(posatt);

    material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse(color);

    //    Qt3DExtras::QPerVertexColorMaterial * mat_obj = new Qt3DExtras::QPerVertexColorMaterial();

    entity = new Qt3DCore::QEntity(rootEntity);
    entity->addComponent(m_obj);
    entity->addComponent(material);
    entity->addComponent(transform);
}

void Object3D::setPosAtt(QPosAtt posatt)
{
    transform->setTranslation(toQVector3D(posatt.P));
    transform->setRotation(toQQuaternion(posatt.Q));
}
