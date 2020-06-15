#include "cloud3d.h"

Cloud3D::Cloud3D(Qt3DCore::QEntity* rootEntity)
{
    cloudTransform = new Qt3DCore::QTransform();
    cloudTransform->setScale(1.0);
    cloudTransform->setRotation(QQuaternion(1,0,0,0));
    cloudTransform->setTranslation(QVector3D(0,0,0));
    geometry = new Qt3DRender::QGeometry(rootEntity);
    buffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,geometry);
    positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buffer);
    positionAttribute->setByteStride(6 * sizeof(float));
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry
    cloudColorsAttribute = new Qt3DRender::QAttribute(geometry);
    cloudColorsAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());
    cloudColorsAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    cloudColorsAttribute->setVertexSize(3);
    cloudColorsAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    cloudColorsAttribute->setBuffer(buffer);
    cloudColorsAttribute->setByteOffset(3 * sizeof(float));
    cloudColorsAttribute->setByteStride(6 * sizeof(float));
    geometry->addAttribute(cloudColorsAttribute);
    cloudMaterial = new Qt3DExtras::QPerVertexColorMaterial(rootEntity);
    geometryRenderer = new Qt3DRender::QGeometryRenderer(rootEntity);
    geometryRenderer->setGeometry(geometry);
    entity = new Qt3DCore::QEntity(rootEntity);
    entity->addComponent(geometryRenderer);
    entity->addComponent(cloudMaterial);
    entity->addComponent(cloudTransform);
    pointSize = new Qt3DRender::QPointSize();
    pointSize->setSizeMode(Qt3DRender::QPointSize::SizeMode::Fixed);
    pointSize->setValue(4.0f);
    lineWidth = new Qt3DRender::QLineWidth();
    lineWidth->setValue(4.0f);
}
