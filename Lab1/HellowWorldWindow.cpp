#include "HellowWorldWindow.h"
#include "ui_HellowWorld.h"

HelloWorldWindow::HelloWorldWindow(int pid, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::HellowWorld),
      pid(pid) {
    ui->setupUi(this);
    ui->pidLabel->setText("PID: " + QString::number(pid));
}

HelloWorldWindow::~HelloWorldWindow() {
    delete ui;
}
