#ifndef RESOURCEWIDGET_H
#define RESOURCEWIDGET_H

#include <QWidget>
#include <QPen>

class QTimer;

namespace QtCharts {
class QChartView;
}

class ResourceWidget : public QWidget {
    Q_OBJECT

public:
    explicit ResourceWidget(QWidget *parent = nullptr);
    virtual ~ResourceWidget() = default;

protected:
    const int CHART_X_RANGE;
    const QRgb COLOR_DARK_BLUE;
    const QRgb COLOR_LIGHT_BLUE;
    const QRgb COLOR_PINK;
    const int PEN_WIDTH;
    QtCharts::QChartView *pChartView;

protected slots:
    virtual void updateSeries() = 0;

private:
    QTimer *pTimer;
};

#endif // RESOURCEWIDGET_H
