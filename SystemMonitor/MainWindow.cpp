#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "SysInfo.h"
#include "CpuWidget.h"
#include "MemSwapWidget.h"
#include "StatusWidget.h"
#include <QTimer>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      pTimer(new QTimer(this)),
      pCpuWidget(new CpuWidget(this)),
      pMemSwapWidget(new MemSwapWidget(this)),
      pStatusWidget(new StatusWidget(pCpuWidget, pMemSwapWidget, this)) {

    ui->setupUi(this);
    ui->resourceTab->layout()->addWidget(pCpuWidget);
    ui->resourceTab->layout()->addWidget(pMemSwapWidget);
    ui->statusBar->layout()->addWidget(pStatusWidget);

    pTimer->setInterval(1000);
    connect(pTimer, &QTimer::timeout, this, &MainWindow::update);
    pTimer->start();

    ui->hostnameLabel->setText(SysInfo::instance().hostname());
    ui->systemVerLabel->setText(SysInfo::instance().systemVer());
    ui->cpuinfoLabel->setText(SysInfo::instance().cpuInfo());
    ui->lastingTimeLabel->setText(SysInfo::instance().lastingTime());
    ui->startingTimeLabel->setText(SysInfo::instance().startingTime());

    QString statusBarMessage = QDateTime::currentDateTime().toString("yyyy-MM-dd ddd hh:mm:ss");
    ui->statusBar->showMessage(statusBarMessage);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::update() {
    QString statusBarMessage = QDateTime::currentDateTime().toString("yyyy-MM-dd ddd hh:mm:ss");
    ui->statusBar->showMessage(statusBarMessage);
    ui->lastingTimeLabel->setText(QString::number(ui->lastingTimeLabel->text()
                                  .left(ui->lastingTimeLabel->text().size() - 1).
                                  toULongLong() + 1) + 's');
}
