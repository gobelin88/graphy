#include "3d_object.h"

Object3D::Object3D(Qt3DCore::QEntity* rootEntity,
                   Qt3DRender::QMesh* m_obj,
                   Object * pobject,
                   PosAtt posatt,
                   float scale,
                   QColor color)
    :Base3D(rootEntity)
{
    this->p_object=pobject;

    material = new Qt3DExtras::QPhongMaterial();
    static_cast<Qt3DExtras::QPhongMaterial*>(material)->setDiffuse(color);

    //    Qt3DExtras::QPerVertexColorMaterial * mat_obj = new Qt3DExtras::QPerVertexColorMaterial();

    entity->addComponent(m_obj);
    entity->addComponent(material);
    entity->addComponent(transformInit(entity));

    setPosAtt(posatt);
    transform->setScale(scale);

}

Object3D::~Object3D()
{
    std::cout<<"Delete Object3D"<<std::endl;
    if(p_object){delete p_object;}
}

void Object3D::setPosAtt(PosAtt posatt)
{
    transform->setTranslation(toQVector3D(posatt.P));
    transform->setRotation(toQQuaternion(posatt.Q));
}
