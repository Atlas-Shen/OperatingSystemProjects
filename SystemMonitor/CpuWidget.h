#ifndef CPUWIDGET_H
#define CPUWIDGET_H

#include "ResourceWidget.h"

namespace QtCharts {
class QLineSeries;
}

class CpuWidget : public ResourceWidget {
    Q_OBJECT

public:
    explicit CpuWidget(QWidget *parent = nullptr);

signals:
    void sendCpuUsage(double);

private slots:
    void updateSeries() override;

private:
    QtCharts::QLineSeries *pSeries;
    qint64 mSeriesX;
};

#endif // CPUWIDGET_H
