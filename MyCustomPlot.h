#pragma once

#include <qcustomplot/qcustomplot.h>
#include <Eigen/Dense>
#include "kdtree_eigen.h"
#include "DataConvert.h"

class MyQCustomPlot:public QCustomPlot
{
public:
    MyQCustomPlot(QWidget * parent)
        :QCustomPlot(parent)
    {

    }

    QList<QCPAbstractPlottable*> plottables() const
    {
        return mPlottables;
    }

    QList<QCPAxis*> selectedAxesPart(QCPAxis::SelectablePart part) const
    {
        QList<QCPAxis*> result, allAxes;
        foreach (QCPAxisRect* rect, axisRects())
        {
            allAxes << rect->axes();
        }

        foreach (QCPAxis* axis, allAxes)
        {
            if (axis->selectedParts() == part)
            {
                result.append(axis);
            }
        }

        return result;
    }

};

class Hack
{
public:
    Hack(MyQCustomPlot* plot)
    {
        scale=new QCPColorScale(plot);
        scale->setType(QCPAxis::atRight);
    }

    ////////////////////////////////////hack
    QCPRange getScalarFieldRange()const
    {
        double min = *std::min_element(scalarFieldData.constBegin(), scalarFieldData.constEnd());
        double max = *std::max_element(scalarFieldData.constBegin(), scalarFieldData.constEnd());

        if(min!=max)
        {
            return QCPRange (min,max);
        }
        else
        {
            return QCPRange (min-1,max+1);
        }
    }

    void setScalarField(const QVector<double>& _scalarFieldData)
    {
        this->scalarFieldData=_scalarFieldData;
        scalarFieldRange=getScalarFieldRange();
        scalarFieldColors=QVector<QRgb>(scalarFieldData.size(),0);
        scale->setDataRange(scalarFieldRange);
    }

    QVector<double> getScalarField()const
    {
        return scalarFieldData;
    }

    void setAlphaField(const QVector<double>& _alphaFieldData)
    {
        this->alphaFieldData=_alphaFieldData;
    }

    QVector<double> getAlphaField()const
    {
        return alphaFieldData;
    }

    void setLabelField(const QVector<QString>& _labelFieldData)
    {
        this->labelFieldData=_labelFieldData;
    }

    QVector<QString> getLabelField()const
    {
        return labelFieldData;
    }

    void setScalarFieldGradientType(int gradient_type)
    {
        scalarFieldGradientType =static_cast<QCPColorGradient::GradientPreset>(gradient_type);
        scale->setGradient( QCPColorGradient(scalarFieldGradientType) );
        updateScalarFieldColor();
    }

    QCPColorGradient::GradientPreset getScalarFieldGradientType()const
    {
        return scalarFieldGradientType;
    }

    void updateScalarFieldColor()
    {
        if (scalarFieldData.size()>0)
        {
            scale->gradient().colorize(scalarFieldData.data(),scale->dataRange(),scalarFieldColors.data(),scalarFieldColors.size());
        }
    }

    QCPColorScale* getColorScale()
    {
        return scale;
    }

protected:
    QVector<QString> labelFieldData;
    QCPColorScale* scale;
    QVector<QRgb> scalarFieldColors;
    QVector<double> scalarFieldData;
    QVector<double> alphaFieldData;
    QCPColorGradient::GradientPreset scalarFieldGradientType;
    QCPRange scalarFieldRange;
};

class MyQCPGraph:public QCPGraph,public Hack
{
public:
    MyQCPGraph(MyQCustomPlot * plot)
        :QCPGraph(plot->xAxis, plot->yAxis),Hack(plot)
    {

    }


};


class MyQCPCurve:public QCPCurve,public Hack
{
public:
    MyQCPCurve(MyQCustomPlot * plot)
        :QCPCurve(plot->xAxis, plot->yAxis),Hack(plot)
    {

    }

    void drawScatterPlot(QCPPainter* painter,
                        const QVector<QPointF>& points,
                        const QVector<QRgb>& scattercolors,
                        const QVector<double>& scatterangles,
                        const QCPScatterStyle& style) const
    {
        // draw scatter point symbols:
        applyScattersAntialiasingHint(painter);

        for (int i=0; i<points.size(); ++i)
        {
            if (!qIsNaN(points.at(i).x()) && !qIsNaN(points.at(i).y()))
            {
                if (i<scattercolors.size())
                {
                    painter->setPen(QPen(scattercolors[i]));
                }
                else
                {
                    painter->setPen(mPen);
                }
                style.drawShape(painter,  points.at(i),scatterangles.at(i));
            }
        }
    }

    void drawScatterPlot(QCPPainter* painter,
                        const QVector<QPointF>& points,
                        const QVector<QRgb>& scattercolors,
                        const QCPScatterStyle& style) const
    {
        // draw scatter point symbols:
        applyScattersAntialiasingHint(painter);

        for (int i=0; i<points.size(); ++i)
        {
            if (!qIsNaN(points.at(i).x()) && !qIsNaN(points.at(i).y()))
            {
                if (i<scattercolors.size())
                {
                    painter->setPen(QPen(scattercolors[i]));
                }
                else
                {
                    painter->setPen(mPen);
                }
                style.drawShape(painter,  points.at(i));
            }
        }
    }

    void getScatters(QVector<QPointF>* scatters,QVector<QRgb>* scatters_colors,QVector<double>* scatters_angles, const QCPDataRange& dataRange, double scatterWidth) const
    {
        if (!scatters)
        {
            return;
        }
        scatters->clear();
        if (scatters_colors!=nullptr)
        {
            scatters_colors->clear();
        }
        if (scatters_angles!=nullptr)
        {
            scatters_angles->clear();
        }
        QCPAxis* keyAxis = mKeyAxis.data();
        QCPAxis* valueAxis = mValueAxis.data();
        if (!keyAxis || !valueAxis)
        {
            qDebug() << Q_FUNC_INFO << "invalid key or value axis";
            return;
        }

        QCPCurveDataContainer::const_iterator begin = mDataContainer->constBegin();
        QCPCurveDataContainer::const_iterator end = mDataContainer->constEnd();
        mDataContainer->limitIteratorsToDataRange(begin, end, dataRange);
        if (begin == end)
        {
            return;
        }
        const int scatterModulo = mScatterSkip+1;
        const bool doScatterSkip = mScatterSkip > 0;
        int endIndex = end-mDataContainer->constBegin();

        QCPRange keyRange = keyAxis->range();
        QCPRange valueRange = valueAxis->range();
        // extend range to include width of scatter symbols:
        keyRange.lower = keyAxis->pixelToCoord(keyAxis->coordToPixel(keyRange.lower)-scatterWidth*keyAxis->pixelOrientation());
        keyRange.upper = keyAxis->pixelToCoord(keyAxis->coordToPixel(keyRange.upper)+scatterWidth*keyAxis->pixelOrientation());
        valueRange.lower = valueAxis->pixelToCoord(valueAxis->coordToPixel(valueRange.lower)-scatterWidth*valueAxis->pixelOrientation());
        valueRange.upper = valueAxis->pixelToCoord(valueAxis->coordToPixel(valueRange.upper)+scatterWidth*valueAxis->pixelOrientation());

        QCPCurveDataContainer::const_iterator it = begin;
        int id=0;
        int itIndex = begin-mDataContainer->constBegin();
        while (doScatterSkip && it != end && itIndex % scatterModulo != 0) // advance begin iterator to first non-skipped scatter
        {
            ++itIndex;
            ++it;
            ++id;
        }
        if (keyAxis->orientation() == Qt::Vertical)
        {
            while (it != end)
            {
                if (!qIsNaN(it->value) && keyRange.contains(it->key) && valueRange.contains(it->value))
                {
                    scatters->append(QPointF(valueAxis->coordToPixel(it->value), keyAxis->coordToPixel(it->key)));
                    if (scatters_colors!=nullptr)
                    {
                        scatters_colors->append(scalarFieldColors[id]);
                    }
                    if (scatters_angles!=nullptr)
                    {
                        scatters_angles->append(alphaFieldData[id]);
                    }
                }
                // advance iterator to next (non-skipped) data point:
                if (!doScatterSkip)
                {
                    ++it;
                    ++id;
                }
                else
                {
                    itIndex += scatterModulo;
                    if (itIndex < endIndex) // make sure we didn't jump over end
                    {
                        it += scatterModulo;
                        id += scatterModulo;
                    }
                    else
                    {
                        it = end;
                        id = endIndex;
                        itIndex = endIndex;
                    }
                }
            }
        }
        else
        {
            while (it != end)
            {
                if (!qIsNaN(it->value) && keyRange.contains(it->key) && valueRange.contains(it->value))
                {
                    scatters->append(QPointF(keyAxis->coordToPixel(it->key), valueAxis->coordToPixel(it->value)));
                    if (scatters_colors!=nullptr)
                    {
                        scatters_colors->append(scalarFieldColors[id]);
                    }
                    if (scatters_angles!=nullptr)
                    {
                        scatters_angles->append(alphaFieldData[id]);
                    }
                }

                // advance iterator to next (non-skipped) data point:
                if (!doScatterSkip)
                {
                    ++it;
                    ++id;
                }
                else
                {
                    itIndex += scatterModulo;
                    if (itIndex < endIndex) // make sure we didn't jump over end
                    {
                        it += scatterModulo;
                        id+=scatterModulo;
                    }
                    else
                    {
                        it = end;
                        id =endIndex ;
                        itIndex = endIndex;
                    }
                }
            }
        }
    }

    void draw(QCPPainter* painter) override
    {
        if (mDataContainer->isEmpty())
        {
            return;
        }

        // allocate line vector:
        QVector<QPointF> lines, scatters;
        QVector<QRgb> scattercolors;
        QVector<double> scatterangles;

        // loop over and draw segments of unselected/selected data:
        QList<QCPDataRange> selectedSegments, unselectedSegments, allSegments;
        getDataSegments(selectedSegments, unselectedSegments);
        allSegments << unselectedSegments << selectedSegments;
        for (int i=0; i<allSegments.size(); ++i)
        {
            bool isSelectedSegment = i >= unselectedSegments.size();

            // fill with curve data:
            QPen finalCurvePen = mPen; // determine the final pen already here, because the line optimization depends on its stroke width
            if (isSelectedSegment && mSelectionDecorator)
            {
                finalCurvePen = mSelectionDecorator->pen();
            }

            QCPDataRange lineDataRange = isSelectedSegment ? allSegments.at(i) : allSegments.at(i).adjusted(-1, 1); // unselected segments extend lines to bordering selected data point (safe to exceed total data bounds in first/last segment, getCurveLines takes care)
            getCurveLines(&lines, lineDataRange, finalCurvePen.widthF());

            // check data validity if flag set:
    #ifdef QCUSTOMPLOT_CHECK_DATA
            for (QCPCurveDataContainer::const_iterator it = mDataContainer->constBegin(); it != mDataContainer->constEnd(); ++it)
            {
                if (QCP::isInvalidData(it->t) ||
                    QCP::isInvalidData(it->key, it->value))
                {
                    qDebug() << Q_FUNC_INFO << "Data point at" << it->key << "invalid." << "Plottable name:" << name();
                }
            }
    #endif

            // draw curve fill:
            applyFillAntialiasingHint(painter);
            if (isSelectedSegment && mSelectionDecorator)
            {
                mSelectionDecorator->applyBrush(painter);
            }
            else
            {
                painter->setBrush(mBrush);
            }
            painter->setPen(Qt::NoPen);
            if (painter->brush().style() != Qt::NoBrush && painter->brush().color().alpha() != 0)
            {
                painter->drawPolygon(QPolygonF(lines));
            }

            // draw curve line:
            if (mLineStyle != lsNone)
            {
                painter->setPen(finalCurvePen);
                painter->setBrush(Qt::NoBrush);
                drawCurveLine(painter, lines);
            }

            // draw scatters:
            QCPScatterStyle finalScatterStyle = mScatterStyle;
            if (isSelectedSegment && mSelectionDecorator)
            {
                finalScatterStyle = mSelectionDecorator->getFinalScatterStyle(mScatterStyle);
            }
            if (!finalScatterStyle.isNone())
            {
                if (scalarFieldData.size()>0)
                {
                    if (alphaFieldData.size()>0)
                    {
                        getScatters(&scatters,&scattercolors,&scatterangles, allSegments.at(i), finalScatterStyle.size());
                        drawScatterPlot(painter, scatters, scattercolors,scatterangles, finalScatterStyle);
                    }
                    else
                    {
                        getScatters(&scatters,&scattercolors,nullptr, allSegments.at(i), finalScatterStyle.size());
                        drawScatterPlot(painter, scatters, scattercolors, finalScatterStyle);
                    }
                }
                else
                {
                    getScatters(&scatters,nullptr,nullptr, allSegments.at(i), finalScatterStyle.size());
                    drawScatterPlot(painter, scatters, scattercolors, finalScatterStyle);
                }

            }
        }

        // draw other selection decoration that isn't just line/scatter pens and brushes:
        if (mSelectionDecorator)
        {
            mSelectionDecorator->drawDecoration(painter, selection());
        }
    }


};

class MyQCPColorMap:public QCPColorMap
{
public:
    MyQCPColorMap(MyQCustomPlot * plot)
        :QCPColorMap(plot->xAxis, plot->yAxis)
    {

    }

    struct MapParams
    {
        enum InterpolationMode
        {
            MODE_NEAREST,
            MODE_WEIGHTED
        };

        uint resolutionX;
        uint resolutionY;
        uint knn;
        InterpolationMode mode;
        Eigen::VectorXd dataX;
        Eigen::VectorXd dataY;
        Eigen::VectorXd dataZ;

        MapParams()
        {
            this->resolutionX=512;
            this->resolutionY=512;
            this->knn=5;
            this->mode=MODE_NEAREST;
        }

        MapParams(uint resolutionX,
                  uint resolutionY,
                  uint knn,
                  InterpolationMode mode)
        {
            this->resolutionX=resolutionX;
            this->resolutionY=resolutionY;
            this->knn=knn;
            this->mode=mode;
        }

        void write(QDataStream & ds)
        {
            ds<<resolutionX;
            ds<<resolutionY;
            ds<<knn;
            ds<<static_cast<int>(mode);

            ds<<dataX;
            ds<<dataY;
            ds<<dataZ;
        }

        void read(QDataStream & ds)
        {
            ds>>resolutionX;
            ds>>resolutionY;
            ds>>knn;
            int modei;
            ds>>modei;
            mode=static_cast<InterpolationMode>(modei);


            ds>>dataX;
            ds>>dataY;
            ds>>dataZ;
        }

        void interpolate(QCPColorMap* map)
        {
            std::cout<<"interpolate A"<<std::endl;
            Eigen::MatrixXd datapoints(2,dataX.size());

            for (int i=0; i<datapoints.cols(); i++)
            {
                datapoints(0,i)=dataX[i];
                datapoints(1,i)=dataY[i];
            }

            //std::cout<<"data[box.idX].size()="<<datapoints.cols()<<std::endl;
            //std::cout<<datapoints.transpose()<<std::endl;

            std::cout<<"interpolate B"<<std::endl;
            kdt::KDTreed kdtree(datapoints);
            kdtree.build();

            kdt::KDTreed::Matrix dists; // basically Eigen::MatrixXd
            kdt::KDTreed::MatrixI idx; // basically Eigen::Matrix<Eigen::Index>

            uint nx=uint(map->data()->keySize());
            uint ny=uint(map->data()->valueSize());

            std::cout<<"interpolate C"<<std::endl;
            //Query points
            kdt::KDTreed::Matrix queryPoints(2,nx*ny);
            int id=0;
            for (uint i=0; i<nx; i++)
            {
                for (uint j=0; j<ny; j++)
                {
                    map->data()->cellToCoord(int(i),int(j),&queryPoints(0,id),&queryPoints(1,id));
                    id++;
                }
            }

            std::cout<<"interpolate D"<<std::endl;
            //Do the job
            if (mode==MapParams::MODE_WEIGHTED)
            {
                kdtree.query(queryPoints, knn, idx, dists);
            }
            else if (mode==MapParams::MODE_NEAREST)
            {
                kdtree.query(queryPoints, 1, idx, dists);
            }

            std::cout<<"interpolate E"<<std::endl;
            //Results
            id=0;
            for (uint i=0; i<nx; i++)
            {
                for (uint j=0; j<ny; j++)
                {
                    double value=0;
                    if (mode==MapParams::MODE_WEIGHTED)
                    {
                        double weight_sum=0;
                        for (uint k=0; k<knn; k++)
                        {
                            if (idx(k,id)>=0 && idx(0,id)<dataZ.rows())
                            {
                                value+=(1.0/dists(k,id))* dataZ[idx(k,id)];
                                weight_sum+=(1.0/dists(k,id));
                            }
                        }
                        value/=weight_sum;
                    }
                    else if (mode==MapParams::MODE_NEAREST)
                    {
                        if (idx(0,id)>=0 && idx(0,id)<dataZ.rows())
                        {
                            value=dataZ[idx(0,id)];
                        }
                    }

                    map->data()->setCell(i,j,value);
                    id++;
                }
            }
        }

        void operator=(const MapParams & other)
        {
            this->resolutionX=other.resolutionX;
            this->resolutionY=other.resolutionY;
            this->knn=other.knn;
            this->mode=other.mode;

            dataX=other.dataX;
            dataY=other.dataY;
            dataZ=other.dataZ;
        }
    };

    const QImage & getImage(){return mMapImage;}

    MapParams mapParams;

};

class QCP_LIB_DECL MyQCPItemCustomCurve : public QCPAbstractItem
{
    Q_OBJECT
public:
    explicit MyQCPItemCustomCurve(QCustomPlot* parentPlot);
    virtual ~MyQCPItemCustomCurve();

    // getters:
    QPen pen() const
    {
        return mPen;
    }
    QPen selectedPen() const
    {
        return mSelectedPen;
    }

    // setters;
    void setPen(const QPen& pen);
    void setSelectedPen(const QPen& pen);

    void setData(const std::vector<Eigen::Vector2d> & points)
    {
        this->points=points;
    }

    virtual double selectTest(const QPointF& pos, bool onlySelectable, QVariant* details=0) const Q_DECL_OVERRIDE;

protected:
    // property members:
    QPen mPen, mSelectedPen;

    // reimplemented virtual methods:
    virtual void draw(QCPPainter* painter) Q_DECL_OVERRIDE;

    QCPItemPosition * currentPoint;

    std::vector<Eigen::Vector2d> points;

    QPainterPath getPath()const;
    QPen mainPen() const;
};



