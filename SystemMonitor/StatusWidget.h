#ifndef STATUSWIDGET_H
#define STATUSWIDGET_H

#include <QWidget>

class CpuWidget;
class MemSwapWidget;

namespace Ui {
class StatusWidget;
}

class StatusWidget : public QWidget {
    Q_OBJECT

public:
    explicit StatusWidget(CpuWidget *, MemSwapWidget *, QWidget *parent = nullptr);
    ~StatusWidget();

public slots:
    void showCpuUsage(double);
    void showMemSwapUsage(double, double);

private:
    Ui::StatusWidget *ui;
    CpuWidget *pCpuWidget;
    MemSwapWidget *pMemSwapWidget;
};

#endif // STATUSWIDGET_H
