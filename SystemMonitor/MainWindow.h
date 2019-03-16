#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class CpuWidget;
class MemSwapWidget;
class StatusWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void update();

private:
    Ui::MainWindow *ui;
    CpuWidget *pCpuWidget;
    MemSwapWidget *pMemSwapWidget;
    StatusWidget *pStatusWidget;
};

#endif // MAINWINDOW_H
