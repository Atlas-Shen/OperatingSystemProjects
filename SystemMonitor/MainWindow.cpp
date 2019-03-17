#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "System.h"
#include "CpuWidget.h"
#include "MemSwapWidget.h"
#include "StatusWidget.h"
#include "ProcessModel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      pCpuWidget(new CpuWidget(this)),
      pMemSwapWidget(new MemSwapWidget(this)),
      pStatusWidget(new StatusWidget(this)),
      pProcessModel(new ProcessModel(this)) {

    ui->setupUi(this);
    connect(&System::instance(), &System::toMainWindow, this, &MainWindow::update);
    ui->resourceTab->layout()->addWidget(pCpuWidget);
    ui->resourceTab->layout()->addWidget(pMemSwapWidget);
    ui->statusBar->layout()->addWidget(pStatusWidget);
    ui->tableView->setModel(pProcessModel);
    ui->tableView->setShowGrid(false);

    ui->hostnameLabel->setText(System::instance().hostname());
    ui->systemVerLabel->setText(System::instance().systemVer());
    ui->cpuinfoLabel->setText(System::instance().cpuInfo());
    ui->startingTimeLabel->setText(System::instance().startingTime());
    update();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::update() {
    ui->lastingTimeLabel->setText(System::instance().lastingTime());
    ui->statusBar->showMessage(QDateTime::currentDateTime().toString("yyyy-MM-dd ddd hh:mm:ss"));
}
