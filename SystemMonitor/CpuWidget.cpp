#include "CpuWidget.h"
#include "System.h"
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>

using namespace QtCharts;

CpuWidget::CpuWidget(QWidget *parent)
    : ResourceWidget(parent),
      pSeries(new QLineSeries(this)),
      mSeriesX(0) {
    QPen pen(COLOR_PINK);
    pen.setWidth(PEN_WIDTH);
    pSeries->setPen(pen);

    QChart *chart = pChartView->chart();
    chart->addSeries(pSeries);
    chart->setTitle("CPU History");
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal, pSeries).first()->setRange(0, CHART_X_RANGE);
    chart->axes(Qt::Vertical, pSeries).first()->setRange(0, 100);

    pSeries->append(mSeriesX++, System::instance().cpuUsage());
}

void CpuWidget::update() {
    pSeries->append(mSeriesX++, System::instance().cpuUsage());
    if (pSeries->count() > CHART_X_RANGE + 1) {
        pSeries->remove(0);
        QChart *chart = pChartView->chart();
        chart->scroll(chart->plotArea().width() / CHART_X_RANGE, 0);
    }
}
