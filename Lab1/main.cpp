#include "HellowWorldWindow.h"
#include <QApplication>
#include <unistd.h>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    HelloWorldWindow w(getpid());
    w.show();

    return a.exec();
}
