#include "HellowWorldWindow.h"
#include "TimeWindow.h"
#include "RandomIntWindow.h"
#include <QApplication>
#include <unistd.h>
#include <wait.h>

int main(int argc, char *argv[]) {
    int pid1 = fork();
    if (pid1 > 0) {
        int pid2 = fork();
        if (pid2 > 0) {
            int pid3 = fork();
            if (pid3 > 0) {
                wait(nullptr);
                wait(nullptr);
                wait(nullptr);
            } else if (pid3 == 0) {
                QApplication a(argc, argv);
                RandomIntWindow w(getpid());
                w.show();
                a.exec();
            }
        } else if (pid2 == 0) {
            QApplication a(argc, argv);
            TimeWindow w(getpid());
            w.show();
            a.exec();
        }
    } else if (pid1 == 0) {
        QApplication a(argc, argv);
        HelloWorldWindow w(getpid());
        w.show();
        a.exec();
    }

    return 0;
}
