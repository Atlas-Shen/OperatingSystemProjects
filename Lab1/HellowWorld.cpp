#include "HellowWorld.h"
#include "ui_HellowWorld.h"

HelloWorld::HelloWorld(int pid, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::HellowWorld),
      pid(pid) {
    ui->setupUi(this);
    ui->label->setText("PID: " + QString::number(pid));
}

HelloWorld::~HelloWorld() {
    delete ui;
}
