

#ifndef BASE3D_H
#define BASE3D_H

#include <Qt3DExtras>
#include <Qt3DRender>
#include <iostream>

class Base3D
{
public:
    Base3D()
    {
        entity=nullptr;
        buffer=nullptr;
        positionAttribute=nullptr;
        geometry=nullptr;
        geometryRenderer=nullptr;

        indexBuffer=nullptr;
        indexAttribute=nullptr;
        material=nullptr;
        transform=nullptr;

        tR.setToIdentity();
        tT.setToIdentity();
    }

    Qt3DCore::QTransform * transformInit(Qt3DCore::QEntity* entity)
    {
        transform=new Qt3DCore::QTransform(entity);
        transform->setScale(1.0);
        transform->setRotation(QQuaternion(1,0,0,0));
        transform->setTranslation(QVector3D(0,0,0));

        return transform;
    }

    virtual ~Base3D()
    {
        if(entity)
        {
            entity->setParent(static_cast<Qt3DCore::QEntity*>(nullptr));
        }
        std::cout<<"~Base3D 1"<<std::endl;
        if(entity)delete entity;

//        std::cout<<"~Base3D 2"<<std::endl;
//        if(buffer)delete buffer;

//        std::cout<<"~Base3D 3"<<std::endl;
//        if(positionAttribute)delete positionAttribute;

//        std::cout<<"~Base3D 4"<<std::endl;
//        if(geometry)delete geometry;

//        std::cout<<"~Base3D 5"<<std::endl;
//        if(geometryRenderer)delete geometryRenderer;

//        std::cout<<"~Base3D 6"<<std::endl;
//        if(indexBuffer)delete indexBuffer;

//        std::cout<<"~Base3D 7"<<std::endl;
//        if(indexAttribute)delete indexAttribute;

//        std::cout<<"~Base3D 8"<<std::endl;
//        if(material)delete material;

//        std::cout<<"~Base3D 9"<<std::endl;
//        if(transform)delete transform;
    }

    Qt3DCore::QEntity* entity;
    Qt3DRender::QBuffer* buffer;
    Qt3DRender::QAttribute* positionAttribute;
    Qt3DRender::QGeometry* geometry;
    Qt3DRender::QGeometryRenderer* geometryRenderer;

    Qt3DRender::QBuffer* indexBuffer;
    Qt3DRender::QAttribute* indexAttribute;
    Qt3DRender::QMaterial * material;//Qt3DExtras::QPhongMaterial Qt3DExtras::QPerVertexColorMaterial Qt3DExtras::QPhongAlphaMaterial

    Qt3DCore::QTransform * transform;
    QMatrix4x4 tR,tT;
};

#endif // BASE3D_H
