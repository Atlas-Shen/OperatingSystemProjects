#include "StatusWidget.h"
#include "ui_StatusWidget.h"
#include "CpuWidget.h"
#include "MemSwapWidget.h"
#include <QString>

StatusWidget::StatusWidget(CpuWidget *p1, MemSwapWidget *p2, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::StatusWidget),
      pCpuWidget(p1),
      pMemSwapWidget(p2) {
    ui->setupUi(this);
    connect(pCpuWidget, &CpuWidget::sendCpuUsage, this, &StatusWidget::showCpuUsage);
    connect(pMemSwapWidget, &MemSwapWidget::sendMemSwapUsage, this, &StatusWidget::showMemSwapUsage);
}

StatusWidget::~StatusWidget() {
    delete ui;
}

void StatusWidget::showCpuUsage(double cpuUsage) {
    ui->cpuUsageLabel->setText("CPU: " + QString::number(cpuUsage, 'f', 2) + '%');
}

void StatusWidget::showMemSwapUsage(double memUsage, double swapUsage) {
    ui->memUsageLabel->setText("Mem: " + QString::number(memUsage, 'f', 2) + '%');
    ui->swapUsageLabel->setText("Swap: " + QString::number(swapUsage, 'f', 2) + '%');
}
