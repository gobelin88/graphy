

#ifndef BASE3D_H
#define BASE3D_H

#include <Qt3DExtras>
#include <Qt3DRender>

struct Base3D
{
    Qt3DCore::QEntity* entity;
    Qt3DRender::QBuffer* buffer;
    Qt3DRender::QAttribute* positionAttribute;
    Qt3DRender::QGeometry* geometry;
    Qt3DRender::QGeometryRenderer* geometryRenderer;
};

#endif // BASE3D_H
