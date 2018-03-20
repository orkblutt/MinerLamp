#include "hashratecharview.h"


hashrateCharView::hashrateCharView(QWidget *parent) :
    QChartView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    setRubberBand( QChartView::HorizontalRubberBand );

}

hashrateCharView::hashrateCharView(QChart *chart, QWidget *parent) :
     QChartView(chart, parent)
{
    setRenderHint(QPainter::Antialiasing);
    setRubberBand( QChartView::HorizontalRubberBand );
}

void hashrateCharView::setChart(QChart *chart)
{
    QChartView::setChart(chart);
}

void hashrateCharView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF point = chart()->mapToValue(event->pos());
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
