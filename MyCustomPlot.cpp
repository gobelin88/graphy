#include "MyCustomPlot.h"

void MyQCPItemCustomCurve::setPen(const QPen& pen)
{
    mPen = pen;
}

void MyQCPItemCustomCurve::setSelectedPen(const QPen& pen)
{
    mSelectedPen = pen;
}

MyQCPItemCustomCurve::MyQCPItemCustomCurve(QCustomPlot* parentPlot):
    QCPAbstractItem(parentPlot)
{
    setPen(QPen(Qt::black));
    setSelectedPen(QPen(Qt::blue,2));
    currentPoint=createPosition("point");
}

MyQCPItemCustomCurve::~MyQCPItemCustomCurve()
{

}

double MyQCPItemCustomCurve::selectTest(const QPointF& pos, bool onlySelectable, QVariant* details) const
{
    Q_UNUSED(details)
    if (onlySelectable && !mSelectable)
    {
        return -1;
    }

    QPainterPath path=getPath();

    QList<QPolygonF> polygons = path.toSubpathPolygons();
    if (polygons.isEmpty())
    {
        return -1;
    }
    const QPolygonF polygon = polygons.first();
    QCPVector2D p(pos);
    double minDistSqr = (std::numeric_limits<double>::max)();
    for (int i=1; i<polygon.size(); ++i)
    {
        double distSqr = p.distanceSquaredToLine(polygon.at(i-1), polygon.at(i));
        if (distSqr < minDistSqr)
        {
            minDistSqr = distSqr;
        }
    }
    return qSqrt(minDistSqr);
}

QPainterPath MyQCPItemCustomCurve::getPath()const
{
    QPainterPath path;
    for(int i=0;i<points.size();i++)
    {
        currentPoint->setCoords(points[i].x(),
                               points[i].y());

        if(i==0){path.moveTo(currentPoint->pixelPosition());}
        else{path.lineTo(currentPoint->pixelPosition());}
    }
    return path;
}

void MyQCPItemCustomCurve::draw(QCPPainter* painter)
{
    QPainterPath path=getPath();
    painter->setPen(mainPen());
    painter->drawPath(path);
}

QPen MyQCPItemCustomCurve::mainPen() const
{
    return mSelected ? mSelectedPen : mPen;
}
