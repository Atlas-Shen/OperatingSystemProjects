#ifndef HELLOWWORLD_H
#define HELLOWWORLD_H

#include <QWidget>

namespace Ui {
class HellowWorld;
}

class HelloWorldWindow : public QWidget {
    Q_OBJECT

public:
    explicit HelloWorldWindow(int pid, QWidget *parent = nullptr);
    ~HelloWorldWindow();

private:
    Ui::HellowWorld *ui;
    int pid;
};

#endif // HELLOWWORLD_H
