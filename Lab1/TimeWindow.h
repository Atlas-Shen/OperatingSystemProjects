#ifndef TIMEWINDOW_H
#define TIMEWINDOW_H

#include <QWidget>

class QTimer;

namespace Ui {
class TimeWindow;
}

class TimeWindow : public QWidget {
    Q_OBJECT

public:
    explicit TimeWindow(int pid, QWidget *parent = nullptr);
    ~TimeWindow();

public slots:
    void update();

private:
    Ui::TimeWindow *ui;
    int pid;
    QTimer *pTimer;
};

#endif // TIMEWINDOW_H
