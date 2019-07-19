#include "view3d.h"

QQuaternion toQQuaternion(Eigen::Quaterniond q){return QQuaternion(q.w(),q.x(),q.y(),q.z());}
QVector3D toQVector3D(Eigen::Vector3d v){return QVector3D(v.x(),v.y(),v.z());}

View3D::View3D()
{
    //renderSettings()->setRenderPolicy(Qt3DRender::QRenderSettings::RenderPolicy::OnDemand);

    defaultFrameGraph()->setClearColor(QColor(0,0,0));
    defaultFrameGraph()->setFrustumCullingEnabled(false);

    container = QWidget::createWindowContainer(this);
    container->setMinimumSize(QSize(512, 512));
    container->setMaximumSize(screen()->size());
    //registerAspect(new Qt3DInput::QInputAspect);

    //Camera
    camera_params=new CameraParams(0,0,0.5);
    Qt3DRender::QCamera *cameraEntity = camera();
    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.01f, 1000.0f);
    camera_params->update(cameraEntity);

    //Racine
    rootEntity = new Qt3DCore::QEntity();
    setRootEntity(rootEntity);

    ///////////////////////////////////////////////////////////////////////////




    ///////////////////////////////////////////////////////////////////////////

    //Culling
//    Qt3DRender::QRenderPass *renderPass = new Qt3DRender::QRenderPass();
//    Qt3DRender::QCullFace *face = new Qt3DRender::QCullFace();
//    face->setMode(Qt3DRender::QCullFace::NoCulling);
//    renderPass->addRenderState(face);

    //Camera controler
    //        Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    //        camController->setCamera(cameraEntity);

    //Light
    Qt3DCore::QEntity *lightEntity_a = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight *light_a = new Qt3DRender::QPointLight(lightEntity_a);
    light_a->setColor("white");
    light_a->setIntensity(1);
    lightEntity_a->addComponent(light_a);
    Qt3DCore::QTransform *lightTransform_a = new Qt3DCore::QTransform(lightEntity_a);
    lightTransform_a->setTranslation(QVector3D(0.5,0.5,-0.5));
    lightEntity_a->addComponent(lightTransform_a);

    Qt3DCore::QEntity *lightEntity_b = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight *light_b = new Qt3DRender::QPointLight(lightEntity_b);
    light_b->setColor("white");
    light_b->setIntensity(1);
    lightEntity_b->addComponent(light_b);
    Qt3DCore::QTransform *lightTransform_b = new Qt3DCore::QTransform(lightEntity_b);
    lightTransform_b->setTranslation(QVector3D(0.5,0.5,0.5));
    lightEntity_b->addComponent(lightTransform_b);

    Qt3DCore::QEntity *lightEntity_c = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight *light_c = new Qt3DRender::QPointLight(lightEntity_c);
    light_c->setColor("white");
    light_c->setIntensity(1);
    lightEntity_c->addComponent(light_c);
    Qt3DCore::QTransform *lightTransform_c = new Qt3DCore::QTransform(lightEntity_c);
    lightTransform_c->setTranslation(QVector3D(0,0.5,0));
    lightEntity_c->addComponent(lightTransform_c);
}

void View3D::addMultiObj(QStringList filenames,QPosAtt posatt,float scale,QList<QColor> color)
{
    QVector<Qt3DRender::QMesh *> m_obj (filenames.size());

    for(int k=0;k<m_obj.size();k++)
    {
        m_obj[k] = new Qt3DRender::QMesh();
        m_obj[k] ->setSource(QUrl(QString("file:///")+filenames[k]));
    }

    Qt3DCore::QTransform *t_obj = new Qt3DCore::QTransform(rootEntity);
    t_obj->setShareable(true);
    t_obj->setScale(scale);
    t_obj->setRotation(toQQuaternion(posatt.Q));
    t_obj->setTranslation(toQVector3D(posatt.P));

    for(int k=0;k<m_obj.size();k++)
    {
        Qt3DCore::QEntity * m_objEntity = new Qt3DCore::QEntity(rootEntity);
        Qt3DExtras::QPhongMaterial * mat_obj = new Qt3DExtras::QPhongMaterial(rootEntity);
        mat_obj->setDiffuse(color[k]);
        m_objEntity->addComponent(m_obj[k]);
        m_objEntity->addComponent(mat_obj);
        m_objEntity->addComponent(t_obj);
    }


    transforms.push_back(t_obj);
}

void View3D::addMultiObj(QStringList filenames,QPosAtt posatt,float scale,QColor color)
{
    QVector<Qt3DRender::QMesh *> m_obj (filenames.size());

    for(int k=0;k<m_obj.size();k++)
    {
        m_obj[k] = new Qt3DRender::QMesh();
        m_obj[k] ->setSource(QUrl(QString("file:///")+filenames[k]));
    }

    Qt3DCore::QTransform *t_obj = new Qt3DCore::QTransform(rootEntity);
    t_obj->setShareable(true);
    t_obj->setScale(scale);
    t_obj->setRotation(toQQuaternion(posatt.Q));
    t_obj->setTranslation(toQVector3D(posatt.P));

    Qt3DCore::QEntity * m_objEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QPhongMaterial * mat_obj = new Qt3DExtras::QPhongMaterial(rootEntity);
    mat_obj->setDiffuse(color);

    for(int k=0;k<m_obj.size();k++)
    {
        m_objEntity->addComponent(m_obj[k]);
        m_objEntity->addComponent(mat_obj);
        m_objEntity->addComponent(t_obj);
    }

    materials.push_back(mat_obj);
    transforms.push_back(t_obj);
}

void View3D::addGrid(float step,unsigned int n,QColor color)
{
    auto * geometry = new Qt3DRender::QGeometry(rootEntity);

    // position vertices (start and end)
    QByteArray bufferBytes;
    bufferBytes.resize(3 * (4*n) * sizeof(float));
    float *positions = reinterpret_cast<float*>(bufferBytes.data());

    float half=(n*step)/2.0;
    for(unsigned int i=0;i<n;i++)
    {
        *positions++ = (i+0.5)*step-half;
        *positions++ = 0;
        *positions++ = half;

        *positions++ = (i+0.5)*step-half;
        *positions++ = 0;
        *positions++ = -half;

        *positions++ = half;
        *positions++ = 0;
        *positions++ = (i+0.5)*step-half;

        *positions++ = -half;
        *positions++ = 0;
        *positions++ = (i+0.5)*step-half;
    }

    auto *buf = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,geometry);
    buf->setData(bufferBytes);

    auto *positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buf);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(4*n);
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize( (4*n) * sizeof(unsigned int)); // start to end
    unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());

    for(unsigned int i=0;i<n;i++)
    {
        *indices++ = 0+4*i;
        *indices++ = 1+4*i;
        *indices++ = 2+4*i;
        *indices++ = 3+4*i;
    }


    auto *indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::BufferType::IndexBuffer,geometry);
    indexBuffer->setData(indexBytes);

    auto *indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(4*n);
    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh
    auto *line = new Qt3DRender::QGeometryRenderer(rootEntity);
    line->setGeometry(geometry);
    line->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    auto *material = new Qt3DExtras::QPhongMaterial(rootEntity);
    material->setAmbient(color);

    // entity
    auto *lineEntity = new Qt3DCore::QEntity(rootEntity);
    lineEntity->addComponent(line);
    lineEntity->addComponent(material);
}

void View3D::addCloudLine(Cloud * cloud,QColor color)
{
    auto * geometry = new Qt3DRender::QGeometry(rootEntity);

    // position vertices (start and end)
    QByteArray bufferBytes;
    bufferBytes.resize(3 * ( cloud->data().size() ) * sizeof(float));
    float *positions = reinterpret_cast<float*>(bufferBytes.data());

    for(int i=0;i<cloud->data().size();i++)
    {
        *positions++ = cloud->data()[i][0];
        *positions++ = cloud->data()[i][1];
        *positions++ = cloud->data()[i][2];
    }

    auto *buf = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,geometry);
    buf->setData(bufferBytes);

    auto *positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buf);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(cloud->data().size());
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize( (cloud->data().size()-1) * sizeof(unsigned int)); // start to end
    unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());

    for(unsigned int i=0;i<cloud->data().size()-1;i++)
    {
        *indices++ = i;
    }

    auto *indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::BufferType::IndexBuffer,geometry);
    indexBuffer->setData(indexBytes);

    auto *indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount( cloud->data().size()-1 );
    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh
    auto *line = new Qt3DRender::QGeometryRenderer(rootEntity);
    line->setGeometry(geometry);
    line->setPrimitiveType(Qt3DRender::QGeometryRenderer::LineStrip);
    auto *material = new Qt3DExtras::QPhongMaterial(rootEntity);
    material->setAmbient( color );

    // entity
    auto *lineEntity = new Qt3DCore::QEntity(rootEntity);
    lineEntity->addComponent(line);
    lineEntity->addComponent(material);
}

void View3D::addCloud(Cloud * cloud,QColor color)
{
    auto * geometry = new Qt3DRender::QGeometry(rootEntity);

    // position vertices (start and end)
    QByteArray bufferBytes;
    bufferBytes.resize(3 * ( cloud->data().size() ) * sizeof(float));
    float *positions = reinterpret_cast<float*>(bufferBytes.data());

    for(int i=0;i<cloud->data().size();i++)
    {
        *positions++ = cloud->data()[i][0];
        *positions++ = cloud->data()[i][1];
        *positions++ = cloud->data()[i][2];
    }

    auto *buf = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,geometry);
    buf->setData(bufferBytes);

    auto *positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buf);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(cloud->data().size());
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

    // mesh
    auto *points = new Qt3DRender::QGeometryRenderer(rootEntity);
    points->setGeometry(geometry);
    points->setPrimitiveType(Qt3DRender::QGeometryRenderer::Points);
    auto *material = new Qt3DExtras::QPhongMaterial(rootEntity);
    material->setAmbient( color );

    // entity
    auto *pointsEntity = new Qt3DCore::QEntity(rootEntity);
    pointsEntity->addComponent(points);
    pointsEntity->addComponent(material);

    //
    auto effect = material->effect();
    for (auto t : effect->techniques()) {
        for (auto rp : t->renderPasses()) {
            auto pointSize = new Qt3DRender::QPointSize();
            pointSize->setSizeMode(Qt3DRender::QPointSize::SizeMode::Fixed);
            pointSize->setValue(4.0f);
            rp->addRenderState(pointSize);
        }
    }
}

void View3D::addCloudScalarLine(CloudScalar * cloud)
{
    auto * geometry = new Qt3DRender::QGeometry(rootEntity);

    // position vertices (start and end)
    QByteArray bufferBytes;
    bufferBytes.resize(2 * 3 * ( cloud->data().size() ) * sizeof(float));
    float *vertices = reinterpret_cast<float*>(bufferBytes.data());

    for(int i=0;i<cloud->data().size();i++)
    {
        *vertices++ = cloud->data()[i][0];
        *vertices++ = cloud->data()[i][1];
        *vertices++ = cloud->data()[i][2];

        *vertices++ = qRed  (cloud->getColors()[i])/255.0;
        *vertices++ = qGreen(cloud->getColors()[i])/255.0;
        *vertices++ = qBlue (cloud->getColors()[i])/255.0;
    }

    auto *buf = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,geometry);
    buf->setData(bufferBytes);

    auto *positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buf);
    positionAttribute->setByteStride(6 * sizeof(float));
    positionAttribute->setCount(cloud->data().size());
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

    // color per vertices (start and end)//////////////////////////////////////////////////////////
    auto *colorsAttribute = new Qt3DRender::QAttribute(geometry);
    colorsAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());
    colorsAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    colorsAttribute->setVertexSize(3);
    colorsAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    colorsAttribute->setBuffer(buf);
    colorsAttribute->setByteOffset(3 * sizeof(float));
    colorsAttribute->setByteStride(6 * sizeof(float));
    colorsAttribute->setCount(cloud->data().size());
    geometry->addAttribute(colorsAttribute); // We add the vertices in the geometry

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize( (cloud->data().size()-1) * sizeof(unsigned int)); // start to end
    unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());

    for(unsigned int i=0;i<cloud->data().size()-1;i++)
    {
        *indices++ = i;
    }

    auto *indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::BufferType::IndexBuffer,geometry);
    indexBuffer->setData(indexBytes);

    auto *indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount( cloud->data().size()-1 );
    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh
    auto *material = new Qt3DExtras::QPerVertexColorMaterial(rootEntity);
    auto *line = new Qt3DRender::QGeometryRenderer(rootEntity);
    line->setGeometry(geometry);
    line->setPrimitiveType(Qt3DRender::QGeometryRenderer::LineStrip);

    // entity
    auto *lineEntity = new Qt3DCore::QEntity(rootEntity);
    lineEntity->addComponent(line);
    lineEntity->addComponent(material);
}

void View3D::addCloudScalar(CloudScalar * cloud)
{
    auto * geometry = new Qt3DRender::QGeometry(rootEntity);

    // position vertices (start and end)//////////////////////////////////////////////////////////
    QByteArray bufferBytes;
    bufferBytes.resize(2 * 3 * ( cloud->data().size() ) * sizeof(float));
    float *vertices = reinterpret_cast<float*>(bufferBytes.data());
    for(int i=0;i<cloud->data().size();i++)
    {
        *vertices++ = cloud->data()[i][0];
        *vertices++ = cloud->data()[i][1];
        *vertices++ = cloud->data()[i][2];

        *vertices++ = qRed  (cloud->getColors()[i])/255.0;
        *vertices++ = qGreen(cloud->getColors()[i])/255.0;
        *vertices++ = qBlue (cloud->getColors()[i])/255.0;
    }
    auto *buf = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,geometry);
    buf->setData(bufferBytes);

    auto *positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buf);
    positionAttribute->setByteStride(6 * sizeof(float));
    positionAttribute->setCount(cloud->data().size());
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

    // color per vertices (start and end)//////////////////////////////////////////////////////////
    auto *colorsAttribute = new Qt3DRender::QAttribute(geometry);
    colorsAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());
    colorsAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    colorsAttribute->setVertexSize(3);
    colorsAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    colorsAttribute->setBuffer(buf);
    colorsAttribute->setByteOffset(3 * sizeof(float));
    colorsAttribute->setByteStride(6 * sizeof(float));
    colorsAttribute->setCount(cloud->data().size());
    geometry->addAttribute(colorsAttribute); // We add the vertices in the geometry

    // mesh
    auto *points = new Qt3DRender::QGeometryRenderer(rootEntity);
    points->setGeometry(geometry);
    points->setPrimitiveType(Qt3DRender::QGeometryRenderer::Points);

    auto *material = new Qt3DExtras::QPerVertexColorMaterial(rootEntity);

    // entity
    auto *pointsEntity = new Qt3DCore::QEntity(rootEntity);
    pointsEntity->addComponent(points);
    pointsEntity->addComponent(material);

    //
    auto effect = material->effect();
    for (auto t : effect->techniques()) {
        for (auto rp : t->renderPasses()) {
            auto pointSize = new Qt3DRender::QPointSize();
            pointSize->setSizeMode(Qt3DRender::QPointSize::SizeMode::Fixed);
            pointSize->setValue(4.0f);
            rp->addRenderState(pointSize);
        }
    }
}

void View3D::addCircle(float radius,unsigned int n,QColor color)
{
    auto * geometry = new Qt3DRender::QGeometry(rootEntity);

    // position vertices (start and end)
    QByteArray bufferBytes;
    bufferBytes.resize(3 * (n) * sizeof(float));
    float *positions = reinterpret_cast<float*>(bufferBytes.data());

    float step=(2.0*M_PI)/n;
    for(unsigned int i=0;i<n;i++)
    {
        float alpha=step*i;
        *positions++ = cos(alpha)*radius;
        *positions++ = 0;
        *positions++ = sin(alpha)*radius;
    }

    auto *buf = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,geometry);
    buf->setData(bufferBytes);

    auto *positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buf);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(n);
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize( (2*n) * sizeof(unsigned int)); // start to end
    unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());

    for(unsigned int i=0;i<n;i++)
    {
        *indices++ = i;
        *indices++ = (i+1)%n;
    }


    auto *indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::BufferType::IndexBuffer,geometry);
    indexBuffer->setData(indexBytes);

    auto *indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(2*n);
    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh
    auto *line = new Qt3DRender::QGeometryRenderer(rootEntity);
    line->setGeometry(geometry);
    line->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    auto *material = new Qt3DExtras::QPhongMaterial(rootEntity);
    material->setAmbient(color);

    // entity
    auto *lineEntity = new Qt3DCore::QEntity(rootEntity);
    lineEntity->addComponent(line);
    lineEntity->addComponent(material);
}

void View3D::drawLine(const QVector3D& start, const QVector3D& end, const QColor& color)
{
    auto *geometry = new Qt3DRender::QGeometry(rootEntity);

    // position vertices (start and end)
    QByteArray bufferBytes;
    bufferBytes.resize(3 * 2 * sizeof(float)); // start.x, start.y, start.end + end.x, end.y, end.z
    float *positions = reinterpret_cast<float*>(bufferBytes.data());
    *positions++ = start.x();
    *positions++ = start.y();
    *positions++ = start.z();
    *positions++ = end.x();
    *positions++ = end.y();
    *positions++ = end.z();

    auto *buf = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer,geometry);
    buf->setData(bufferBytes);

    auto *positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buf);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(2);
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize(2 * sizeof(unsigned int)); // start to end
    unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());
    *indices++ = 0;
    *indices++ = 1;

    auto *indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::BufferType::IndexBuffer,geometry);
    indexBuffer->setData(indexBytes);

    auto *indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(2);
    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh
    auto *line = new Qt3DRender::QGeometryRenderer(rootEntity);
    line->setGeometry(geometry);
    line->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    auto *material = new Qt3DExtras::QPhongMaterial(rootEntity);
    material->setAmbient(color);

    // entity
    auto *lineEntity = new Qt3DCore::QEntity(rootEntity);
    lineEntity->addComponent(line);
    lineEntity->addComponent(material);
}

void View3D::addObj(QString filename, QPosAtt posatt,float scale,QColor color)
{
    auto * m_obj = new Qt3DRender::QMesh();
    m_obj->setSource(QUrl(QString("file:///")+filename));

    auto *t_obj = new Qt3DCore::QTransform();
    t_obj->setScale(scale);
    t_obj->setRotation(toQQuaternion(posatt.Q));
    t_obj->setTranslation(toQVector3D(posatt.P));

    auto * mat_obj = new Qt3DExtras::QPhongMaterial();
    mat_obj->setDiffuse(color);

    //    Qt3DExtras::QPerVertexColorMaterial * mat_obj = new Qt3DExtras::QPerVertexColorMaterial();

    auto * m_objEntity = new Qt3DCore::QEntity(rootEntity);
    m_objEntity->addComponent(m_obj);
    m_objEntity->addComponent(mat_obj);
    m_objEntity->addComponent(t_obj);

    transforms.push_back(t_obj);
    materials.push_back(mat_obj);
}

void View3D::setObjColor(int id,QColor color)
{
    if(id<materials.size())
    {
        materials[id]->setDiffuse(color);
    }
}

void View3D::setObjPosAtt(int id,const QPosAtt & T)
{
    if(id<transforms.size())
    {
        transforms[id]->setTranslation(toQVector3D(T.P)*1e-3);
        transforms[id]->setRotation(toQQuaternion(T.Q));
    }
}

QWidget * View3D::getContainer()
{
    return container;
}

void View3D::mouseMoveEvent(QMouseEvent * event)
{
    if(event->buttons()==Qt::LeftButton)
    {
        double dx=xp-event->x();
        double dy=yp-event->y();

        camera_params->move(camera(),dx,dy);

        xp=event->x();
        yp=event->y();
    }
}

void View3D::mouseDoubleClickEvent(QMouseEvent * event)
{
    if(event->buttons()==Qt::LeftButton)
    {
        camera_params->moveTo(camera(),0,0,0.5);
    }
}

void View3D::mousePressEvent(QMouseEvent * event)
{
    if(event->buttons()==Qt::LeftButton)
    {
        xp=event->x();
        yp=event->y();
    }
}
void View3D::wheelEvent(QWheelEvent * event)
{
    double dw=event->delta();
    if(dw>0)
    {
        if(camera_params->radius<1.0)
            camera_params->radius*=dw*0.01;
    }
    else
    {
        if(camera_params->radius>0.1)
            camera_params->radius/=-dw*0.01;
    }

    camera_params->update(camera());
}
