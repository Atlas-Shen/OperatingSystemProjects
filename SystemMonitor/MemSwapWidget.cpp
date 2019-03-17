#include "MemSwapWidget.h"
#include "System.h"
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>

using namespace QtCharts;

MemSwapWidget::MemSwapWidget(QWidget *parent)
    : ResourceWidget(parent),
      pMemSeries(new QLineSeries(this)),
      pSwapSeries(new QLineSeries(this)),
      mMemSeriesX(0),
      mSwapSeriesX(0) {
    QPen memPen(COLOR_DARK_BLUE);
    memPen.setWidth(PEN_WIDTH);
    pMemSeries->setPen(memPen);
    QPen swapPen(COLOR_LIGHT_BLUE);
    swapPen.setWidth(PEN_WIDTH);
    pSwapSeries->setPen(swapPen);

    QChart *chart = pChartView->chart();
    chart->addSeries(pMemSeries);
    chart->addSeries(pSwapSeries);
    chart->setTitle("Memory and Swap History");
    chart->createDefaultAxes();
    //chart->axes(Qt::Horizontal, mSeries).first()->setVisible(false);
    chart->axes(Qt::Horizontal, pMemSeries).first()->setRange(0, CHART_X_RANGE);
    chart->axes(Qt::Vertical, pMemSeries).first()->setRange(0, 100);

    pMemSeries->append(mMemSeriesX++, System::instance().memUsage());
    pSwapSeries->append(mSwapSeriesX++, System::instance().swapUsage());
}

void MemSwapWidget::update() {
    pMemSeries->append(mMemSeriesX++, System::instance().memUsage());
    pSwapSeries->append(mSwapSeriesX++, System::instance().swapUsage());
    if (pMemSeries->count() > CHART_X_RANGE + 1) {
        pMemSeries->remove(0);
        pSwapSeries->remove(0);
        QChart *chart = pChartView->chart();
        chart->scroll(chart->plotArea().width() / CHART_X_RANGE, 0);
    }
}
