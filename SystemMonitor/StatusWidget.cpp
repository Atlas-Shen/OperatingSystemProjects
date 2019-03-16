#include "StatusWidget.h"
#include "ui_StatusWidget.h"
#include "SysInfo.h"
#include <QString>

StatusWidget::StatusWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::StatusWidget) {
    ui->setupUi(this);
    connect(&SysInfo::instance(), &SysInfo::toStatusWidget, this, &StatusWidget::update);
    update();
}

StatusWidget::~StatusWidget() {
    delete ui;
}

void StatusWidget::update() {
    ui->cpuUsageLabel->setText(QString("CPU: %1%").arg(SysInfo::instance().cpuUsage(), 0, 'f', 2));
    ui->memUsageLabel->setText(QString("Mem: %1%").arg(SysInfo::instance().memUsage(), 0, 'f', 2));
    ui->swapUsageLabel->setText(QString("Swap: %1%").arg(SysInfo::instance().swapUsage(), 0, 'f', 2));
}
