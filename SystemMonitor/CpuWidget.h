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

private slots:
    void update() override;

private:
    QtCharts::QLineSeries *pSeries;
    qint64 seriesX;
};

#endif // CPUWIDGET_H
