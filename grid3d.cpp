#include "grid3d.h"

Grid3D::Grid3D(Qt3DCore::QEntity* rootEntity,unsigned int N,QColor color)
{
    geometry = new Qt3DRender::QGeometry(rootEntity);

    unsigned int n=(N+1)*3;

    buffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,geometry);
    buffer->setData(getGridBuffer(false,false,false,N));

    positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buffer);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(4*n);
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize( (4*n) * sizeof(unsigned int)); // start to end
    unsigned int* indices = reinterpret_cast<unsigned int*>(indexBytes.data());
    for (unsigned int i=0; i<n; i++)
    {
        *indices++ = 0+4*i;
        *indices++ = 1+4*i;
        *indices++ = 2+4*i;
        *indices++ = 3+4*i;
    }

    gridIndexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::BufferType::IndexBuffer,geometry);
    gridIndexBuffer->setData(indexBytes);

    gridIndexAttribute = new Qt3DRender::QAttribute(geometry);
    gridIndexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    gridIndexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    gridIndexAttribute->setBuffer(gridIndexBuffer);
    gridIndexAttribute->setCount(4*n);
    geometry->addAttribute(gridIndexAttribute); // We add the indices linking the points in the geometry

    // mesh
    geometryRenderer = new Qt3DRender::QGeometryRenderer(rootEntity);
    geometryRenderer->setGeometry(geometry);
    geometryRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    gridMaterial = new Qt3DExtras::QPhongMaterial(rootEntity);
    gridMaterial->setAmbient(color);

    // entity
    entity = new Qt3DCore::QEntity(rootEntity);
    entity->addComponent(geometryRenderer);
    entity->addComponent(gridMaterial);
}

QByteArray Grid3D::getGridBuffer(bool xy_swap,bool xz_swap,bool yz_swap,int N)
{
    unsigned int n=(N+1)*3;

    QByteArray bufferBytes;
    bufferBytes.resize(3 * (4*n) * sizeof(float));
    float* positions = reinterpret_cast<float*>(bufferBytes.data());

    double step=2.0/N;

    for (unsigned int i=0; i<N+1; i++)
    {
        *positions++ = i*step-1;
        *positions++ = xz_swap?1:-1;
        *positions++ = 1;

        *positions++ = i*step-1;
        *positions++ = xz_swap?1:-1;
        *positions++ = -1;

        *positions++ = 1;
        *positions++ = xz_swap?1:-1;
        *positions++ = i*step-1;

        *positions++ = -1;
        *positions++ = xz_swap?1:-1;
        *positions++ = i*step-1;
    }

    for (unsigned int i=0; i<N+1; i++)
    {
        *positions++ = yz_swap?1:-1;
        *positions++ = (i)*step-1;
        *positions++ = 1;

        *positions++ = yz_swap?1:-1;
        *positions++ = i*step-1;
        *positions++ = -1;

        *positions++ = yz_swap?1:-1;
        *positions++ = 1;
        *positions++ = i*step-1;

        *positions++ = yz_swap?1:-1;
        *positions++ = -1;
        *positions++ = (i)*step-1;
    }

    for (unsigned int i=0; i<N+1; i++)
    {
        *positions++ = (i)*step-1;
        *positions++ = 1;
        *positions++ = xy_swap?1:-1;

        *positions++ = (i)*step-1;
        *positions++ = -1;
        *positions++ = xy_swap?1:-1;

        *positions++ = 1;
        *positions++ = (i)*step-1;
        *positions++ = xy_swap?1:-1;

        *positions++ = -1;
        *positions++ = (i)*step-1;
        *positions++ = xy_swap?1:-1;
    }

    return bufferBytes;
}
