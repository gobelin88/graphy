

#ifndef BASE3D_H
#define BASE3D_H

#include <Qt3DExtras>
#include <Qt3DRender>

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
        transform=nullptr;
    }

    virtual ~Base3D()
    {
        if(entity)delete entity;
        if(buffer)delete buffer;
        if(positionAttribute)delete positionAttribute;
        if(geometry)delete geometry;
        if(geometryRenderer)delete geometryRenderer;
        if(transform)delete transform;
    }

    Qt3DCore::QEntity* entity;
    Qt3DRender::QBuffer* buffer;
    Qt3DRender::QAttribute* positionAttribute;
    Qt3DRender::QGeometry* geometry;
    Qt3DRender::QGeometryRenderer* geometryRenderer;
    Qt3DCore::QTransform* transform;
};

#endif // BASE3D_H
