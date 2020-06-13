#include "label3d.h"

Label3D::Label3D(Qt3DCore::QEntity* rootEntity,
                 QString text,
                 QVector3D position,
                 float scale,
                 float anglex,
                 float angley,
                 float anglez)
{
    entity = new Qt3DCore::QEntity();
    entity->setParent(rootEntity);

    textMaterial = new Qt3DExtras::QPhongMaterial(rootEntity);
    textMaterial->setDiffuse(QColor(0,0,0));

    textTransform = new Qt3DCore::QTransform();
    textTransform->setTranslation(position);
    textTransform->setRotationX(anglex);
    textTransform->setRotationY(angley);
    textTransform->setRotationZ(anglez);
    textTransform->setScale(scale);

    textMesh = new Qt3DExtras::QExtrudedTextMesh();
    textMesh->setText(text);
    textMesh->setDepth(.001f);

    entity->addComponent(textMaterial);
    entity->addComponent(textTransform);
    entity->addComponent(textMesh);
}

void Label3D::setPosRot(QVector3D position,float anglex,
                        float angley,
                        float anglez)
{
    textTransform->setTranslation(position);
    textTransform->setRotationX(anglex);
    textTransform->setRotationY(angley);
    textTransform->setRotationZ(anglez);
}

void Label3D::setPos(QVector3D position)
{
    textTransform->setTranslation(position);
}

void Label3D::setText(QString text)
{
    textMesh->setText(text);
}
