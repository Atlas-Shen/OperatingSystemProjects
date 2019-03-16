#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "SysInfo.h"
#include "CpuWidget.h"
#include "MemSwapWidget.h"
#include "StatusWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      pCpuWidget(new CpuWidget(this)),
      pMemSwapWidget(new MemSwapWidget(this)),
      pStatusWidget(new StatusWidget(this)) {

    connect(&SysInfo::instance(), &SysInfo::toMainWindow, this, &MainWindow::update);

    ui->setupUi(this);
    ui->resourceTab->layout()->addWidget(pCpuWidget);
    ui->resourceTab->layout()->addWidget(pMemSwapWidget);
    ui->statusBar->layout()->addWidget(pStatusWidget);

    ui->hostnameLabel->setText(SysInfo::instance().hostname());
    ui->systemVerLabel->setText(SysInfo::instance().systemVer());
    ui->cpuinfoLabel->setText(SysInfo::instance().cpuInfo());
    ui->startingTimeLabel->setText(SysInfo::instance().startingTime());

    update();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::update() {
    ui->lastingTimeLabel->setText(SysInfo::instance().lastingTime());
    ui->statusBar->showMessage(QDateTime::currentDateTime().toString("yyyy-MM-dd ddd hh:mm:ss"));
}
