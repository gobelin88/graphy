

#ifndef BASE3D_H
#define BASE3D_H

#include <Qt3DExtras>
#include <Qt3DRender>
#include <iostream>

class Base3D:public QObject
{
Q_OBJECT
public:
    Base3D(Qt3DCore::QEntity* rootEntity)
    {
        entity= new Qt3DCore::QEntity(rootEntity);
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

        picker=new Qt3DRender::QObjectPicker(entity);
        picker->setHoverEnabled(false);
        picker->setEnabled(true);
        connect(picker, &Qt3DRender::QObjectPicker::clicked,this,&Base3D::slot_pick);
        entity->addComponent(picker);
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

public slots:
    void slot_pick(Qt3DRender::QPickEvent* event)
    {
        std::cout<<"picked :("<<entity<<")"<<event->distance()<<std::endl;
    }

public:
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

    Qt3DRender::QObjectPicker * picker;
};

#endif // BASE3D_H
