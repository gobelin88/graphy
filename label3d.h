#ifndef LABEL3D_H
#define LABEL3D_H

#include <Qt3DExtras>
#include <Qt3DRender>

struct Label3D
{
    Label3D(Qt3DCore::QEntity* rootEntity,
            QString text,
            QVector3D position,
            float scale,
            float anglex,
            float angley,
            float anglez);

    void setPosRot(QVector3D position,
                   float anglex,
                   float angley,
                   float anglez);

    void setPos(QVector3D position);
    void setText(QString text);

    Qt3DCore::QEntity* entity;
    Qt3DExtras::QPhongMaterial* textMaterial;
    Qt3DCore::QTransform* textTransform;
    Qt3DExtras::QExtrudedTextMesh* textMesh;
};

#endif // LABEL3D_H
