#include "cloud3d.h"

Cloud3D::Cloud3D(Cloud *cloud, Qt3DCore::QEntity* rootEntity)
    :Base3D(rootEntity)
{
    this->cloud=cloud;

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
    indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    indexAttribute->setVertexSize(3);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    indexAttribute->setBuffer(buffer);
    indexAttribute->setByteOffset(3 * sizeof(float));
    indexAttribute->setByteStride(6 * sizeof(float));
    geometry->addAttribute(indexAttribute);
    material = new Qt3DExtras::QPerVertexColorMaterial(rootEntity);
    geometryRenderer = new Qt3DRender::QGeometryRenderer(rootEntity);
    geometryRenderer->setGeometry(geometry);

    entity->addComponent(geometryRenderer);
    entity->addComponent(material);
    entity->addComponent(transformInit(entity));
    pointSize = new Qt3DRender::QPointSize();
    pointSize->setSizeMode(Qt3DRender::QPointSize::SizeMode::Fixed);
    pointSize->setValue(4.0f);
    lineWidth = new Qt3DRender::QLineWidth();
    lineWidth->setValue(4.0f);

    positionAttribute->setCount(static_cast<unsigned int>(cloud->size()));
    indexAttribute->setCount(static_cast<unsigned int>(cloud->size()));

    buffer->setData(cloud->getBuffer(cloud->getScalarFieldRange()));
}
