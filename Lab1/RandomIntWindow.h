#ifndef RANDOMINTWINDOW_H
#define RANDOMINTWINDOW_H

#include <QWidget>

class QTimer;

namespace Ui {
class RandomIntWindow;
}

class RandomIntWindow : public QWidget {
    Q_OBJECT

public:
    explicit RandomIntWindow(int pid, QWidget *parent = nullptr);
    ~RandomIntWindow();

public slots:
    void update();

private:
    Ui::RandomIntWindow *ui;
    int pid;
    QTimer *pTimer;
};

#endif // RANDOMINTWINDOW_H
