#include "TimeWindow.h"
#include "ui_TimeWindow.h"
#include <QTimer>
#include <QDateTime>
#include <QString>

TimeWindow::TimeWindow(int pid, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::TimeWindow),
      pid(pid),
      pTimer(new QTimer(this)) {
    ui->setupUi(this);
    ui->pidLabel->setText("PID: " + QString::number(pid));
    pTimer->setInterval(1000);
    connect(pTimer, &QTimer::timeout, this, &TimeWindow::update);
    pTimer->start();
    update();
}

TimeWindow::~TimeWindow() {
    delete ui;
}

void TimeWindow::update() {
    ui->timeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd ddd hh:mm:ss"));
}
