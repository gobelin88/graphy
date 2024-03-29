#include "3d_shapes.h"

Grid3D::Grid3D(Qt3DCore::QEntity* rootEntity,unsigned int N,QColor color)
:Base3D(rootEntity)
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

    indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::BufferType::IndexBuffer,geometry);
    indexBuffer->setData(indexBytes);

    indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(4*n);
    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh
    geometryRenderer = new Qt3DRender::QGeometryRenderer(rootEntity);
    geometryRenderer->setGeometry(geometry);
    geometryRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    material = new Qt3DExtras::QPhongMaterial(rootEntity);
    static_cast<Qt3DExtras::QPhongMaterial*>(material)->setAmbient(color);

    // entity
    entity->addComponent(geometryRenderer);
    entity->addComponent(material);
}

QByteArray Grid3D::getGridBuffer(bool xy_swap,bool xz_swap,bool yz_swap,unsigned int N)
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

////////////////////////////
Plan3D::Plan3D(Qt3DCore::QEntity* rootEntity,Plan* plan,float radius,QColor color)
:Base3D(rootEntity)
{
    this->plan=plan;

    geometry = new Qt3DRender::QGeometry(rootEntity);

    buffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,geometry);
    buffer->setData(getBuffer(radius));

    positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buffer);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(4);
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize( 6 * sizeof(unsigned int)); // start to end
    unsigned int* indices = reinterpret_cast<unsigned int*>(indexBytes.data());
    *indices++ = 0;
    *indices++ = 1;
    *indices++ = 2;
    *indices++ = 0;
    *indices++ = 2;
    *indices++ = 3;


    indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::BufferType::IndexBuffer,geometry);
    indexBuffer->setData(indexBytes);

    indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(6);
    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh
    geometryRenderer = new Qt3DRender::QGeometryRenderer(rootEntity);
    geometryRenderer->setGeometry(geometry);
    geometryRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    material = getShapeFitMaterial(rootEntity,color);

    // entity
    entity->addComponent(geometryRenderer);
    entity->addComponent(material);
    entity->addComponent(transformInit(entity));
}
Plan3D::~Plan3D()
{
    std::cout<<"Delete Plan3D"<<std::endl;
    delete plan;
}
QByteArray Plan3D::getBuffer(float radius)
{
    QByteArray bufferBytes;
    bufferBytes.resize(3 * 4 * sizeof(float));
    float* positions = reinterpret_cast<float*>(bufferBytes.data());

    Eigen::Matrix3d m=plan->getStableBase();

    for (int i=0; i<4; i++)
    {
        double alpha=i/4.0*2.0*M_PI;
        Eigen::Vector3d p= radius*m.col(1)*cos(alpha)+radius*m.col(2)*sin(alpha)+plan->getBarycenter();

        *positions++ = p.x();
        *positions++ = p.y();
        *positions++ = p.z();
    }

    return bufferBytes;
}

////////////////////////////
Sphere3D::Sphere3D(Qt3DCore::QEntity* rootEntity,Sphere * sphere,QColor color)
:Base3D(rootEntity)
{
    this->sphere=sphere;

    geometry = new Qt3DRender::QGeometry(rootEntity);

    buffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,geometry);
    buffer->setData(getBuffer());

    positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buffer);
    positionAttribute->setByteStride(6 * sizeof(float));
    positionAttribute->setCount(Na*Nb);
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

    normalAttribute = new Qt3DRender::QAttribute(geometry);
    normalAttribute->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());
    normalAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    normalAttribute->setVertexSize(3);
    normalAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    normalAttribute->setBuffer(buffer);
    normalAttribute->setByteOffset(3 * sizeof(float));
    normalAttribute->setByteStride(6 * sizeof(float));
    geometry->addAttribute(normalAttribute);

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize( Na*Nb*2*3 * sizeof(unsigned int)); // start to end
    unsigned int* indices = reinterpret_cast<unsigned int*>(indexBytes.data());
    for(int i=0;i<Na;i++)
    {
        for(int j=0;j<Nb;j++)
        {
            int idb=((j+1)==Nb)?Nb-1:j+1;
            int ida=(i+1)%Na;

            *indices++ = j+i*Nb;
            *indices++ = idb+i*Nb;
            *indices++ = j+ida*Nb;
            *indices++ = idb+i*Nb;
            *indices++ = idb+ida*Nb;
            *indices++ = j+ida*Nb;
        }
    }


    indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::BufferType::IndexBuffer,geometry);
    indexBuffer->setData(indexBytes);

    indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(Na*Nb*2*3);
    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh
    geometryRenderer = new Qt3DRender::QGeometryRenderer(rootEntity);
    geometryRenderer->setGeometry(geometry);
    geometryRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    material = getShapeFitMaterial(rootEntity,color);

    // entity
    entity->addComponent(geometryRenderer);
    entity->addComponent(material);
    entity->addComponent(transformInit(entity));
}

Sphere3D::~Sphere3D()
{
    std::cout<<"Delete Sphere3D"<<std::endl;
    delete sphere;
}

QByteArray Sphere3D::getBuffer()
{
    QByteArray bufferBytes;
    bufferBytes.resize(Na* Nb*6 * sizeof(float));
    float* positions = reinterpret_cast<float*>(bufferBytes.data());

    double radius=sphere->getRadius();

    for (int i=0; i<Na; i++)
    {
        for (int j=0; j<Nb; j++)
        {
            double alpha=double(i)/Na*M_PI*2;
            double beta=double(j)/(Nb-1)*M_PI-M_PI/2;


            Eigen::Vector3d n(cos(alpha)*cos(beta),sin(alpha)*cos(beta),sin(beta));
            Eigen::Vector3d p= sphere->getCenter()+radius*n;

            *positions++ = p.x();
            *positions++ = p.y();
            *positions++ = p.z();

            *positions++ = n.x();
            *positions++ = n.y();
            *positions++ = n.z();
        }
    }

    return bufferBytes;
}

////////////////////////////
Ellipsoid3D::Ellipsoid3D(Qt3DCore::QEntity* rootEntity,Ellipsoid * ellipsoid,QColor color)
:Base3D(rootEntity)
{
    this->ellipsoid=ellipsoid;

    geometry = new Qt3DRender::QGeometry(rootEntity);

    buffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,geometry);
    buffer->setData(getBuffer());

    positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buffer);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(Na*Nb);
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize( Na*Nb*2*3 * sizeof(unsigned int)); // start to end
    unsigned int* indices = reinterpret_cast<unsigned int*>(indexBytes.data());
    for(int i=0;i<Na;i++)
    {
        for(int j=0;j<Nb;j++)
        {
            int idb=((j+1)==Nb)?Nb-1:j+1;
            int ida=(i+1)%Na;

            *indices++ = j+i*Nb;
            *indices++ = idb+i*Nb;
            *indices++ = j+ida*Nb;
            *indices++ = idb+i*Nb;
            *indices++ = idb+ida*Nb;
            *indices++ = j+ida*Nb;
        }
    }


    indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::BufferType::IndexBuffer,geometry);
    indexBuffer->setData(indexBytes);

    indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(Na*Nb*2*3);
    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh
    geometryRenderer = new Qt3DRender::QGeometryRenderer(rootEntity);
    geometryRenderer->setGeometry(geometry);
    geometryRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    material = getShapeFitMaterial(rootEntity,color);

    // entity
    entity->addComponent(geometryRenderer);
    entity->addComponent(material);
    entity->addComponent(transformInit(entity));
}

Ellipsoid3D::~Ellipsoid3D()
{
    std::cout<<"Delete Ellisoid 3D"<<std::endl;
    delete ellipsoid;
}

QByteArray Ellipsoid3D::getBuffer()
{
    QByteArray bufferBytes;
    bufferBytes.resize(Na* Nb*3 * sizeof(float));
    float* positions = reinterpret_cast<float*>(bufferBytes.data());

    double A=ellipsoid->getA();
    double B=ellipsoid->getB();
    double C=ellipsoid->getC();

    for (int i=0; i<Na; i++)
    {
        for (int j=0; j<Nb; j++)
        {
            double alpha=double(i)/Na*M_PI*2;
            double beta=double(j)/(Nb-1)*M_PI-M_PI/2;

            Eigen::Vector3d p= ellipsoid->getCenter()+ellipsoid->getR()*Eigen::Vector3d(A*cos(alpha)*cos(beta),B*sin(alpha)*cos(beta),C*sin(beta));

            *positions++ = p.x();
            *positions++ = p.y();
            *positions++ = p.z();
        }
    }

    return bufferBytes;
}
