#include "ResourceWidget.h"
#include <QTimer>
#include <QtCharts/QChartView>
#include <QVBoxLayout>

ResourceWidget::ResourceWidget(QWidget *parent)
    : QWidget(parent),
      CHART_X_RANGE(120),
      COLOR_DARK_BLUE(0x409090),
      COLOR_LIGHT_BLUE(0x61afef),
      COLOR_PINK(0xe575a7),
      PEN_WIDTH(3),
      pChartView(new QtCharts::QChartView(this)),
      pTimer(new QTimer(this)) {
    pTimer->setInterval(1000);
    connect(pTimer, &QTimer::timeout, this, &ResourceWidget::updateSeries);
    pTimer->start();

    pChartView->setRenderHint(QPainter::Antialiasing);
    pChartView->chart()->legend()->setVisible(false);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(pChartView);
    setLayout(layout);
}
