#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class CpuWidget;
class MemSwapWidget;
class StatusWidget;
class ProcessModel;

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
    void sortProcess(int);
    void refreshProcess();
    void searchProcess();
    void endProcess();

private:
    Ui::MainWindow *ui;
    CpuWidget *pCpuWidget;
    MemSwapWidget *pMemSwapWidget;
    StatusWidget *pStatusWidget;
    ProcessModel *pProcessModel;
};

#endif // MAINWINDOW_H
