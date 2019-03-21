#include "HellowWorld.h"
#include <QApplication>
#include <unistd.h>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    HelloWorld w(getpid());
    w.show();

    return a.exec();
}
