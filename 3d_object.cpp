#include "3d_object.h"

Object3D::Object3D(Qt3DCore::QEntity* rootEntity,
                   Qt3DRender::QMesh* m_obj,
                   Object * objet,
                   PosAtt posatt,
                   float scale,
                   QColor color)
    :Base3D(rootEntity)
{
    this->objet=objet;

    material = new Qt3DExtras::QPhongMaterial();
    static_cast<Qt3DExtras::QPhongMaterial*>(material)->setDiffuse(color);

    //    Qt3DExtras::QPerVertexColorMaterial * mat_obj = new Qt3DExtras::QPerVertexColorMaterial();

    entity->addComponent(m_obj);
    entity->addComponent(material);
    entity->addComponent(transformInit(entity));

    setPosAtt(posatt);
    transform->setScale(scale);

}

void Object3D::setPosAtt(PosAtt posatt)
{
    transform->setTranslation(toQVector3D(posatt.P));
    transform->setRotation(toQQuaternion(posatt.Q));
}
