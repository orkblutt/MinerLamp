#include <QDateTimeAxis>
#include <QToolTip>

#include "hashratecharview.h"


hashrateCharView::hashrateCharView(QWidget *parent) :
    QChartView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);
    setInteractive(true);
    setRubberBand(RectangleRubberBand);

}

hashrateCharView::hashrateCharView(QChart *chart, QWidget *parent) :
    QChartView(chart, parent)
{
    setRenderHint(QPainter::Antialiasing);
    //setMouseTracking(true);
    //setInteractive(true);
    setRubberBand(QChartView::NoRubberBand);
}

void hashrateCharView::setChart(QChart *chart)
{
    QChartView::setChart(chart);
}

void hashrateCharView::mouseMoveEvent(QMouseEvent *event)
{

    QPointF point = chart()->mapToValue(event->pos());

    QDateTimeAxis *axisX = qobject_cast<QDateTimeAxis *>(chart()->axisX());


    QToolTip::showText(event->globalPos(),
                       QString::number(point.y()),
                       this, rect() );

    QChartView::mouseMoveEvent(event);

}


void hashrateCharView::wheelEvent(QWheelEvent *event)
{
    if(event->angleDelta().y() > 0)
    {
        chart()->zoomIn();

    }
    else
        chart()->zoomOut();

    event->accept();
}
