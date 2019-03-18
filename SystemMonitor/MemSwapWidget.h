#ifndef MEMWIDGET_H
#define MEMWIDGET_H

#include "ResourceWidget.h"

namespace QtCharts {
class QLineSeries;
}

class MemSwapWidget : public ResourceWidget {
    Q_OBJECT

public:
    explicit MemSwapWidget(QWidget *parent = nullptr);

private slots:
    void update() override;

private:
    QtCharts::QLineSeries *pMemSeries;
    QtCharts::QLineSeries *pSwapSeries;
    qint64 memSeriesX;
    qint64 swapSeriesX;
};

#endif // MEMWIDGET_H
