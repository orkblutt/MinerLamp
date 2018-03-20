#ifndef HASHRATECHARVIEW_H
#define HASHRATECHARVIEW_H

#include <QChartView>
#include <QDateTime>

QT_CHARTS_USE_NAMESPACE


class hashrateCharView : public QChartView
{
public:
    hashrateCharView(QWidget* parent = Q_NULLPTR);
    hashrateCharView(QChart *chart, QWidget *parent = Q_NULLPTR);

    void setChart(QChart *chart);
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event);
private:
    QDateTime _startTime;
};

#endif // HASHRATECHARVIEW_H
