#ifndef HELLOWWORLD_H
#define HELLOWWORLD_H

#include <QWidget>

namespace Ui {
class HellowWorld;
}

class HelloWorld : public QWidget {
    Q_OBJECT

public:
    explicit HelloWorld(int pid, QWidget *parent = nullptr);
    ~HelloWorld();

private:
    Ui::HellowWorld *ui;
    int pid;
};

#endif // HELLOWWORLD_H
